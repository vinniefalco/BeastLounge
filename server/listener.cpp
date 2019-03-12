//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "listener.hpp"
#include "http_session.hpp"
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>
#include <iostream>

// Accepts incoming connections and launches the sessions
class listener
    : public std::enable_shared_from_this<listener>
    , public boost::asio::coroutine
    , public server::agent
{
    server& srv_;
    listener_config cfg_;
    logger::section& log_;
    net::basic_socket_acceptor<
        tcp, server::executor_type> acceptor_;
    tcp::endpoint ep_;

public:
    listener(
        server& srv,
        listener_config cfg)
        : srv_(srv)
        , cfg_(std::move(cfg))
        , log_(srv_.log().get_section("listener"))
        , acceptor_(srv_.make_executor())
    {
        cfg_.kind = listener_config::no_tls;
    }

    bool
    open()
    {
        beast::error_code ec;
        tcp::endpoint ep(cfg_.address, cfg_.port_num);

        // Open the acceptor
        acceptor_.open(ep.protocol(), ec);
        if(ec)
        {
            srv_.log().cerr() <<
                "acceptor_.open: " << ec.message() << "\n";
            return false;
        }

        // Allow address reuse
        acceptor_.set_option(
            net::socket_base::reuse_address(true));
        if(ec)
        {
            srv_.log().cerr() <<
                "acceptor_.set_option: " << ec.message() << "\n";
            return false;
        }

        // Bind to the server address
        acceptor_.bind(ep, ec);
        if(ec)
        {
            srv_.log().cerr() <<
                "acceptor_.bind: " << ec.message() << "\n";
            return false;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec)
        {
            srv_.log().cerr() <<
                "acceptor_.listen: " << ec.message() << "\n";
            return false;
        }

        // Add this agent to the server
        srv_.insert(shared_from_this());

        return true;
    }

    /// Called when the server starts
    void
    on_start() override
    {
        acceptor_.async_accept(
            srv_.make_executor(),
            ep_,
            self(this));
    }

    /// Called when the server stops
    void
    on_stop() override
    {
        // Call do_stop from within the strand
        net::post(
            acceptor_.get_executor(),
            beast::bind_front_handler(
                &listener::do_stop,
                shared_from_this()));
    }

    void
    do_stop()
    {
        // Cancel the outstanding accept
        beast::error_code ec;
        acceptor_.cancel(ec);
    }

    void
    operator()(
        beast::error_code ec,
        tcp::socket sock)
    {
        reenter(*this)
        {
            for(;;)
            {
                if(ec == net::error::operation_aborted)
                {
                    // Happens when the acceptor is canceled
                    return;
                }

                // Report any error
                if(ec)
                    fail(ec, "accept");

            #if 0
                // Launch a new session for this connection
                std::make_shared<http_session>(
                    std::move(sock),
                    state_)->run();
            #endif

                // Accept another connection,
                // giving it a new strand.

                yield acceptor_.async_accept(
                    srv_.make_executor(),
                    ep_,
                    self(this));
            }
        }
    }

    // Report a failure
    void
    fail(beast::error_code ec, char const* what)
    {
        // Don't report on canceled operations
        if(ec == net::error::operation_aborted)
            return;
        LOG_INF(log_, what, ": ", ec.message());
    }
};

//------------------------------------------------------------------------------

bool
make_listener(
    server& srv,
    listener_config cfg)
{
    auto sp =
        std::make_shared<listener>(
            srv, std::move(cfg));
    return sp->open();
}
