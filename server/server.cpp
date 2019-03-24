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
#include "system_channel.hpp"
#include "utility.hpp"
#include <boost/beast/_experimental/json/assign_string.hpp>
#include <boost/beast/_experimental/json/assign_vector.hpp>
#include <boost/beast/_experimental/json/parse_file.hpp>
#include <boost/beast/_experimental/json/parser.hpp>
#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/asio/basic_signal_set.hpp>
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
    using clock_type = std::chrono::steady_clock;
    using time_point = clock_type::time_point;

    server_config cfg_;
    std::shared_ptr<logger> log_;
    std::vector<std::unique_ptr<service>> services_;
    net::basic_waitable_timer<
        clock_type,
        boost::asio::wait_traits<clock_type>,
        executor_type> timer_;
    asio::basic_signal_set<executor_type> signals_;
    std::condition_variable cv_;
    std::mutex mutex_;
    time_point shutdown_time_;
    bool running_ = false;
    bool stop_ = false;

    std::unique_ptr<::dispatcher> dispatcher_;
    ::system_channel system_channel_;

    static
    std::chrono::steady_clock::time_point
    never() noexcept
    {
        return (time_point::max)();
    }

public:
    explicit
    server_impl(
        server_config cfg,
        std::shared_ptr<logger> log)
        : cfg_(std::move(cfg))
        , log_(std::move(log))
        , timer_(this->make_executor())
        , signals_(
            timer_.get_executor(),
            SIGINT,
            SIGTERM)
        , dispatcher_(make_dispatcher(*this))
        , shutdown_time_(never())
    {
        timer_.expires_at(never());
    }

    ~server_impl()
    {
        BOOST_ASSERT(services_.empty());
    }

    void
    insert(std::unique_ptr<service> sp) override
    {
        if(running_)
            throw std::logic_error(
                "server already running");

        services_.emplace_back(std::move(sp));
    }

    void
    run() override
    {
        if(running_)
            throw std::logic_error(
                "server already running");

        running_ = true;

        // Start all agents
        for(auto const& sp : services_)
            sp->on_start();

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        signals_.async_wait(
            bind_front(this, &server_impl::on_signal));

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
        auto agents = std::move(services_);
        for(auto const& sp : agents)
            sp->on_stop();

        // services must be kept alive until after
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

    //--------------------------------------------------------------------------
    //
    // shutdown / stop
    //
    //--------------------------------------------------------------------------

    bool
    is_shutting_down() override
    {
        return false;
    }

    void
    shutdown(std::chrono::seconds cooldown) override
    {
        if(! timer_.get_executor().running_in_this_thread())
            return net::post(
                timer_.get_executor(),
                beast::bind_front_handler(
                    &server_impl::shutdown,
                    this,
                    cooldown));

        // Can't shutdown twice
        if(timer_.expiry() != never())
            return;

        shutdown_time_ = clock_type::now() + cooldown;
        on_timer();
    }

    void
    notify_shutdown()
    {
    }

    void
    on_timer(beast::error_code ec = {})
    {
        if(ec == net::error::operation_aborted)
            return;

        auto const remain =
            ::ceil<std::chrono::seconds>(
                shutdown_time_ - clock_type::now());

        // Countdown finished?
        if(remain.count() <= 0)
        {
            stop();
            return;
        }

        std::chrono::seconds amount(remain.count());
        if(amount.count() > 10)
            amount = std::chrono::seconds(10);

        // Notify users of impending shutdown
        json::value jv;
        jv["verb"] = "say";
        jv["channel"] = system_channel_.cid();
        jv["name"] = system_channel_.name();
        jv["message"] = "Server is shutting down in " +
            std::to_string(remain.count()) + " seconds";

        system_channel_.send(jv);
        timer_.expires_after(amount);
        timer_.async_wait(bind_front(
            this, &server_impl::on_timer));
    }

    void
    on_signal(beast::error_code ec, int signum)
    {
        if(ec == net::error::operation_aborted)
            return;

        log_->cerr() <<
            "server_impl::on_signal: #" <<
            signum << ", " << ec.message() << "\n";
        if(timer_.expiry() == never())
        {
            // Capture signals again
            signals_.async_wait(
                bind_front(this, &server_impl::on_signal));

            this->shutdown(std::chrono::seconds(30));
        }
        else
        {
            // second time hard stop
            stop();
        }
    }

    void
    stop()
    {
        // Get on the strand
        if(! timer_.get_executor().running_in_this_thread())
            return net::post(
                timer_.get_executor(),
                bind_front(this, &server_impl::stop));

        // Only call once
        if(stop_)
            return;

        // Set stop_ and unblock the main thread
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
            cv_.notify_all();
        }

        // Cancel our outstanding I/O
        timer_.cancel();
        beast::error_code ec;
        signals_.cancel(ec);
    }

    //--------------------------------------------------------------------------

    beast::string_view
    doc_root() const override
    {
        return cfg_.doc_root;
    }

    logger&
    log() noexcept override
    {
        return *log_;
    }

    ::dispatcher&
    dispatcher() override
    {
        return *dispatcher_;
    }

    ::system_channel&
    system_channel() override
    {
        return system_channel_;
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

