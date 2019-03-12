//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//
 
#include "server.hpp"
#include "listener.hpp"
#include <boost/beast/_experimental/json/assign_string.hpp>
#include <boost/beast/_experimental/json/assign_vector.hpp>
#include <boost/beast/_experimental/json/parse_file.hpp>
#include <boost/beast/_experimental/json/parser.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/make_unique.hpp>
#include <boost/throw_exception.hpp>
#include <iostream>
#include <thread>
#include <vector>

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
            jv.raw_string().c_str(), ec);
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
    }
};

//------------------------------------------------------------------------------

class server_impl
    : public server
{
    server_config cfg_;
    net::io_context ioc_;
    std::shared_ptr<logger> log_;
    std::vector<std::shared_ptr<agent>> agents_;
    asio::signal_set signals_;
    bool started_ = false;

public:
    explicit
    server_impl(
        server_config cfg,
        std::shared_ptr<logger> log)
        : cfg_(std::move(cfg))
        , log_(std::move(log))
        , signals_(ioc_, SIGINT, SIGTERM)
    {
    }

    ~server_impl()
    {
    }

    executor_type
    make_executor() override
    {
        return net::make_strand(
            net::system_executor{});
    }

    void
    insert(
        std::shared_ptr<agent> sp) override
    {
        if(started_)
            throw std::logic_error(
                "server already started");

        agents_.emplace_back(std::move(sp));
    }

    void
    run() override
    {
        if(started_)
            throw std::logic_error(
                "server already started");

        started_ = true;

        // Capture SIGINT and SIGTERM to perform a clean shutdown
        signals_.async_wait(
            beast::bind_front_handler(
                &server_impl::on_signal, this));

        // Start all agents
        for(auto const& sp : agents_)
            sp->on_start();

        // Spawn num_threads-1 worker threads
        std::vector<std::thread> v;
        while(v.size() < cfg_.num_threads - 1)
            v.emplace_back([this]{ do_run(); });

        // Use the caller's thread for the last worker
        do_run();

        // If we get here, then the server has
        // stopped, so join the threads before
        // destroying them.

        for(auto& t : v)
            t.join();
    }

    void
    do_run()
    {
        try
        {
            ioc_.run();
        }
        catch(std::exception const& e)
        {
            boost::ignore_unused(e);
            // log e
            throw;
        }
    }

    void
    on_signal(beast::error_code ec, int signum)
    {
        boost::ignore_unused(ec, signum);
        stop();
    }

    void
    stop()
    {
        {
            beast::error_code ec;
            signals_.cancel(ec);
        }

        auto agents = std::move(agents_);
        for(auto const& sp : agents)
            sp->on_stop();
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

    // Create listeners
    {
        auto& ja = jv["listeners"];
        if(! ja.is_array())
        {
            ec = json::error::expected_array;
            return nullptr;
        }
        for(auto& e : ja.raw_array())
        {
            listener_config cfg;
            e.assign(cfg, ec);
            if(ec)
            {
                log->cerr() <<
                    "listener_config: " << ec.message() << "\n";
                return nullptr;
            }
            if(! make_listener(*srv, std::move(cfg)))
                return nullptr;
        }
    }

    return srv;
}

