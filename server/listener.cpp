//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "listener.hpp"
#include "server_certificate.hpp"
#include <boost/beast/core/detect_ssl.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>
#include <iostream>

extern
void
run_http_session(
    server& srv,
    agent& ag,
    stream_type stream,
    endpoint_type ep,
    flat_storage storage);

extern
void
run_https_session(
    server& srv,
    agent& ag,
    asio::ssl::context& ctx,
    stream_type stream,
    endpoint_type ep,
    flat_storage storage);

namespace {

// Detects the SSL opening handshake and launches either
// a plain HTTP session or a Secure HTTP session.

class detector
    : public boost::enable_shared_from_this<detector>
    , public boost::asio::coroutine
    , public session
{
    server& srv_;
    agent& ag_;
    section& log_;
    asio::ssl::context& ctx_;
    stream_type stream_;
    endpoint_type ep_;
    flat_storage storage_;

public:
    detector(
        server& srv,
        agent& ag,
        section& log,
        asio::ssl::context& ctx,
        socket_type sock,
        endpoint_type ep)
        : srv_(srv)
        , ag_(ag)
        , log_(log)
        , ctx_(ctx)
        , stream_(std::move(sock))
        , ep_(ep)
    {
        ag_.insert(this);
    }

    ~detector()
    {
        ag_.erase(this);
    }

    void
    run()
    {
        // Use post to get on to our strand.
        net::post(
            stream_.get_executor(),
            self(this));
    }

    boost::weak_ptr<session>
    get_weak_session_ptr() override
    {
        return this->weak_from_this();
    }

    void
    on_stop() override
    {
        net::post(
            stream_.get_executor(),
            beast::bind_front_handler(
                &detector::do_stop,
                shared_from_this()));
    }

    void
    do_stop()
    {
        // Cancel pending I/O, this causes an immediate
        // completion with error::operation_aborted.
        stream_.cancel();
    }

    void
    operator()(
        beast::error_code ec = {},
        bool is_tls = false)
    {
        reenter(*this)
        {
            // Set the expiration
            stream_.expires_after(
                std::chrono::seconds(30));

            // See if a TLS handshake is requested
            yield beast::async_detect_ssl(
                stream_,
                storage_,
                self(this));

            // Report any error
            if(ec)
                return fail(ec, "async_detect_ssl");

            if(is_tls)
            {
                // launch the HTTPS session
                return run_https_session(
                    srv_,
                    ag_,
                    ctx_,
                    std::move(stream_),
                    ep_,
                    std::move(storage_));
            }
            else
            {
                // launch the plain HTTP session
                return run_http_session(
                    srv_,
                    ag_,
                    std::move(stream_),
                    ep_,
                    std::move(storage_));
            }
        }
    }

    void
    fail(beast::error_code ec, char const* what)
    {
        if(ec == net::error::operation_aborted)
            LOG_TRC(log_, what, '\t', ec.message());
        else
            LOG_INF(log_, what, '\t', ec.message());
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
    section& log_;
    asio::ssl::context ctx_;
    net::basic_socket_acceptor<
        tcp_ex, executor_type> acceptor_;
    endpoint_type ep_;

public:
    listener(
        server& srv,
        listener_config cfg)
        : srv_(srv)
        , cfg_(std::move(cfg))
        , log_(srv_.log().get_section("listener"))
        , ctx_(asio::ssl::context::tlsv12)
        , acceptor_(srv_.make_executor())
    {
        cfg_.kind = listener_config::allow_tls;

        // This holds the self-signed certificate used by the server
        load_server_certificate(ctx_);
    }

    bool
    open()
    {
        beast::error_code ec;
        endpoint_type ep(cfg_.address, cfg_.port_num);

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
                    run_http_session(
                        srv_,
                        *this,
                        stream_type(std::move(sock)),
                        ep_,
                        {});
                }
                else if(cfg_.kind == listener_config::allow_tls)
                {
                    auto sp = boost::make_shared<detector>(
                        srv_,
                        *this,
                        log_,
                        ctx_,
                        std::move(sock),
                        ep_);
                    sp->run();
                }
                else
                {
                    run_https_session(
                        srv_,
                        *this,
                        ctx_,
                        stream_type(std::move(sock)),
                        ep_,
                        {});
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
        ja.as_array().push_back({});
        auto& jv = ja.as_array().back();
        jv.emplace_object();
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
run_listener(
    server& srv,
    listener_config cfg)
{
    auto sp =
        boost::make_shared<listener>(
            srv, std::move(cfg));
    return sp->open();
}
