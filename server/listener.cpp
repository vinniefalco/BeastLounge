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
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>
#include <iostream>

namespace {

// Detects the SSL opening handshake and launches either
// a plain HTTP session or a Secure HTTP session.

class detector
    : public std::enable_shared_from_this<detector>
    , public boost::asio::coroutine
{
    server& srv_;
    stream_type stream_;

public:
    detector(
        server& srv,
        socket_type sock)
        : srv_(srv)
        , stream_(std::move(sock))
    {
    }
};

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
class listener
    : public boost::enable_shared_from_this<listener>
    , public boost::asio::coroutine
    , public agent
{
    // This hack works around a bug in basic_socket_acceptor
    // which uses the wrong socket type here:
    // https://github.com/boostorg/asio/blob/c7bbd30491c377ebe12f6e33a0992a3280d71fa4/include/boost/asio/detail/reactive_socket_accept_op.hpp#L198

    struct tcp_ex : net::ip::tcp
    {
        tcp_ex(tcp const& t)
            : tcp(t)
        {
        }

        using socket =
            net::basic_stream_socket<
                tcp, executor_type>;
    };

    server& srv_;
    listener_config cfg_;
    logger::section& log_;
    net::basic_socket_acceptor<
        tcp_ex, executor_type> acceptor_;
    tcp::endpoint ep_;
    flat_storage storage_;

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

    void
    do_stop()
    {
        LOG_TRC(log_, "listener::do_stop");

        // Close the acceptor
        beast::error_code ec;
        acceptor_.close();

        // Stop all the sessions
        for(auto& e : release_sessions())
            if(auto sp = e.lock())
                sp->on_stop();
    }

    void
    operator()(
        beast::error_code ec,
        socket_type sock)
    {
        reenter(*this)
        {
            for(;;)
            {
                // Report the error, if any
                if(ec)
                    return fail(ec, "listener::acceptor_.async_accept");

                // If the acceptor is closed it means we stopped
                if(! acceptor_.is_open())
                    return;

                // Launch a new session for this connection
                if(cfg_.kind == listener_config::no_tls)
                {
                    auto sp = make_http_session(
                        srv_,
                        *this,
                        stream_type(std::move(sock)),
                        ep_,
                        std::move(storage_));
                    sp->run();
                }
                else if(cfg_.kind == listener_config::allow_tls)
                {
                }
                else
                {
                    // auto sp = make_https_session
                }

                // Accept the next connection
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
        if(ec == net::error::operation_aborted)
            LOG_TRC(log_, what, '\t', ec.message());
        else
            LOG_INF(log_, what, '\t', ec.message());
    }

    //--------------------------------------------------------------------------
    //
    // agent
    //

    /// Called when the server starts
    void
    on_start() override
    {
        // Accept the first connection
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
    on_stat(json::value& ja) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ja.raw_array().push_back({});
        auto& jv = ja.raw_array().back();
        jv = json::object;
        jv["type"] = "listener";
        jv["name"] = cfg_.name;
        jv["address"] = cfg_.address.to_string();
        jv["port_num"] = cfg_.port_num;
        jv["sessions"] = sessions_.size();
    }
};

} // (anon)

//------------------------------------------------------------------------------

bool
make_listener(
    server& srv,
    listener_config cfg)
{
    auto sp =
        boost::make_shared<listener>(
            srv, std::move(cfg));
    return sp->open();
}
