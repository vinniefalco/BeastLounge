//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//
 
#include "dispatcher.hpp"
#include "listener.hpp"
#include "logger.hpp"
#include "server.hpp"
#include "service.hpp"
#include <boost/beast/_experimental/json/assign_string.hpp>
#include <boost/beast/_experimental/json/assign_vector.hpp>
#include <boost/beast/_experimental/json/parse_file.hpp>
#include <boost/beast/_experimental/json/parser.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/assert.hpp>
#include <boost/make_unique.hpp>
#include <boost/throw_exception.hpp>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

//------------------------------------------------------------------------------

extern
void
make_chat_service(server&);

extern
void
make_ident_service(server&);

extern
std::unique_ptr<dispatcher>
make_dispatcher(server& srv);

//------------------------------------------------------------------------------

namespace boost {
namespace beast {
namespace json {

template<>
struct value_exchange<net::ip::address>
{
    static
    void
    assign(
        net::ip::address& t,
        json::value const& jv,
        error_code& ec)
    {
        if(! jv.is_string())
        {
            ec = json::error::expected_string;
            return;
        }
        t = net::ip::make_address(
            jv.as_string().c_str(), ec);
    }
};

} // json
} // beast
} // boost

void
assign(
    logger_config& cfg,
    json::value const& jv,
    beast::error_code& ec)
{
    auto& jo = jv["log"];
    if(! jo.is_object())
    {
        ec = json::error::expected_object;
        return;
    }
    jo["path"].assign(cfg.path, ec);
    if(ec)
        return;
}

void
assign(
    listener_config& cfg,
    json::value const& jv,
    beast::error_code& ec)
{
    if(! jv.is_object())
    {
        ec = json::error::expected_object;
        return;
    }
    jv["name"].assign(cfg.name, ec);
    if(ec)
        return;
    jv["address"].assign(cfg.address, ec);
    if(ec)
        return;
    jv["port_num"].assign(cfg.port_num, ec);
    if(ec)
        return;
}

//------------------------------------------------------------------------------

namespace {

struct server_config
{
    unsigned num_threads = 1;
    std::string doc_root;

    void
    assign(
        json::value const& jv,
        beast::error_code& ec)
    {
        if(! jv.is_object())
        {
            ec = json::error::expected_object;
            return;
        }
        jv["threads"].assign(num_threads, ec);
        if(ec)
            return;
        if(num_threads < 1)
            num_threads = 1;
        jv["doc-root"].assign(doc_root, ec);
        if(ec)
            return;
    }
};

//------------------------------------------------------------------------------

class server_impl_base : public server
{
public:
    net::io_context ioc_;

    // This function is in a base class because `server_impl`
    // needs to call it from the ctor-initializer list, which
    // would be undefined if the member function was in the
    // derived class.

    executor_type
    make_executor() override
    {
    #ifdef LOUNGE_USE_SYSTEM_EXECUTOR
        return net::make_strand(
            net::system_executor{});
    #else
        return net::make_strand(
            ioc_.get_executor());
    #endif
    }
};

class server_impl
    : public server_impl_base
{
    server_config cfg_;
    std::shared_ptr<logger> log_;
    std::vector<
        boost::shared_ptr<service>> agents_;
    asio::signal_set signals_;
    std::condition_variable cv_;
    std::mutex mutex_;
    bool running_ = false;
    bool stop_ = false;

    std::unique_ptr<::dispatcher> dispatcher_;

public:
    explicit
    server_impl(
        server_config cfg,
        std::shared_ptr<logger> log)
        : cfg_(std::move(cfg))
        , log_(std::move(log))
        , signals_(
            this->make_executor(),
                SIGINT,
                SIGTERM)
        , dispatcher_(make_dispatcher(*this))
    {
    }

    ~server_impl()
    {
        BOOST_ASSERT(agents_.empty());
    }

    void
    insert(
        boost::shared_ptr<service> sp) override
    {
        if(running_)
            throw std::logic_error(
                "server already running");

        agents_.emplace_back(std::move(sp));
    }

    void
    run() override
    {
        if(running_)
            throw std::logic_error(
                "server already running");

        running_ = true;

        // Start all agents
        for(auto const& sp : agents_)
            sp->on_start();

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        signals_.async_wait(
            beast::bind_front_handler(
                &server_impl::on_signal, this));

    #ifndef LOUNGE_USE_SYSTEM_EXECUTOR
        std::vector<std::thread> vt;
        while(vt.size() < cfg_.num_threads)
            vt.emplace_back(
                [this]
                {
                    this->ioc_.run();
                });
    #endif
        // Block the main thread until stop() is called
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]{ return stop_; });
        }

        // Notify all agents to stop
        auto agents = std::move(agents_);
        for(auto const& sp : agents)
            sp->on_stop();

        // agents must be kept alive until after
        // all executor threads are joined.

        // If we get here, then the server has
        // stopped, so join the threads before
        // destroying them.

    #ifdef LOUNGE_USE_SYSTEM_EXECUTOR
        net::system_executor{}.context().join();
    #else
        for(auto& t : vt)
            t.join();
    #endif
    }

    void
    on_signal(beast::error_code ec, int signum)
    {
        log_->cerr() <<
            "server_impl::on_signal: #" <<
            signum << ", " << ec.message() << "\n";
        stop();
    }

    void
    stop()
    {
        net::post(
            signals_.get_executor(),
            beast::bind_front_handler(
                &server_impl::on_stop, this));
    }

    void
    on_stop()
    {
        // only call once
        if(stop_)
            return;

        // Set stop_ and unblock the main thread
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
            cv_.notify_all();
        }

        // Cancel the signal wait operation
        {
            beast::error_code ec;
            signals_.cancel(ec);
        }
    }

    beast::string_view
    doc_root() const override
    {
        return cfg_.doc_root;
    }

    void
    stat(json::value& jv) override
    {
        jv.emplace_array();
        std::lock_guard<std::mutex> lock(mutex_);
        for(auto& ag : agents_)
            ag->on_stat(jv);
    }

    //--------------------------------------------------------------------------

    ::dispatcher&
    dispatcher() override
    {
        return *dispatcher_;
    }

    logger&
    log() noexcept override
    {
        return *log_;
    }
};

} // (anon)

//------------------------------------------------------------------------------

std::unique_ptr<server>
make_server(
    char const* config_path,
    std::unique_ptr<logger> log)
{
    beast::error_code ec;
    // Read the JSON configuration file
    json::value jv;
    {
        json::parser p;
        json::parse_file(config_path, p, ec);
        if(ec)
        {
            log->cerr() <<
                "json::parse_file: " << ec.message() << "\n";
            return nullptr;
        }
        jv = p.release();
    }

    // Read the log configuration
    {
        logger_config cfg;
        jv.assign(cfg, ec);
        if(ec)
        {
            log->cerr() <<
                "logger_config: " << ec.message() << "\n";
            return nullptr;
        }
        if(! log->open(std::move(cfg)))
            return false;
    }

    // Read the server configuration
    std::unique_ptr<server_impl> srv;
    {
        auto& jo = jv["server"];
        if(! jo.is_object())
        {
            ec = json::error::expected_object;
            log->cerr() <<
                "server_config: " << ec.message() << "\n";
            return nullptr;
        }
        server_config cfg;
        jo.assign(cfg, ec);
        if(ec)
        {
            log->cerr() <<
                "server_config: " << ec.message() << "\n";
            return nullptr;
        }

        // Create the server
        srv = boost::make_unique<server_impl>(
            std::move(cfg),
            std::move(log));
    }

    // Add services
    make_chat_service(*srv);
    make_ident_service(*srv);

    // Create listeners
    {
        auto& ja = jv["listeners"];
        if(! ja.is_array())
        {
            ec = json::error::expected_array;
            return nullptr;
        }
        for(auto& e : ja.as_array())
        {
            listener_config cfg;
            e.assign(cfg, ec);
            if(ec)
            {
                srv->log().cerr() <<
                    "listener_config: " << ec.message() << "\n";
                return nullptr;
            }
            if(! run_listener(*srv, std::move(cfg)))
                return nullptr;
        }
    }

    return srv;
}

