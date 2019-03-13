//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "server.hpp"
#include "session.hpp"
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/yield.hpp>
#include <iostream>

//------------------------------------------------------------------------------

namespace {

template<class Derived>
class ws_session_base
    : public asio::coroutine
    , public session
{
protected:
    server& srv_;
    agent& ag_;
    section& log_;
    endpoint_type ep_;
    flat_storage msg_;

public:
    ws_session_base(
        server& srv,
        agent& ag,
        endpoint_type ep)
        : srv_(srv)
        , ag_(ag)
        , log_(srv_.log().get_section("ws_session"))
        , ep_(ep)
    {
        ag_.insert(this);
    }

    ~ws_session_base()
    {
        ag_.remove(this);
    }

    // The CRTP pattern
    Derived*
    impl()
    {
        return static_cast<Derived*>(this);
    }

    void
    run(websocket::request_type req)
    {
        // Apply settings to stream
        impl()->ws().set_option(
            websocket::stream_base::timeout::suggested(
                beast::role_type::server));

        // Limit the maximum incoming message size
        impl()->ws().read_message_max(64 * 1024);

        // TODO check credentials in `req`

        // Perform the WebSocket handshake in the server role
        impl()->ws().async_accept(std::move(req), self(impl()));
    }

    void
    operator()(
        beast::error_code ec = {},
        std::size_t bytes_transferred = 0)
    {
        boost::ignore_unused(bytes_transferred);
        reenter(*this)
        {
            // Report any handshaking errors
            if(ec)
                return fail(ec, "async_accept");

            for(;;)
            {
                // Read the next message
                yield impl()->ws().async_read(
                    msg_, self(impl()));

                // Report any errors reading
                if(ec)
                    return fail(ec, "async_read");

                yield impl()->ws().async_write(
                    msg_.data(), self(impl()));

                // Report any errors writing
                if(ec)
                    return fail(ec, "async_write");

                msg_.clear();
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
    // session
    //

    boost::weak_ptr<session>
    get_weak_ptr() override
    {
        return impl()->weak_from_this();
    }

    void
    on_stop() override
    {
        net::post(
            impl()->ws().get_executor(),
            beast::bind_front_handler(
                &ws_session_base::do_stop,
                impl()->shared_from_this()));
    }

    void
    do_stop()
    {
        beast::error_code ec;
        beast::close_socket(
            beast::get_lowest_layer(impl()->ws()));
    }
};

//------------------------------------------------------------------------------

class plain_ws_session_impl
    : public boost::enable_shared_from_this<
        plain_ws_session_impl>
    , public ws_session_base<
        plain_ws_session_impl>
{
    websocket::stream<stream_type> ws_;

public:
    plain_ws_session_impl(
        server& srv,
        agent& ag,
        stream_type stream,
        endpoint_type ep)
        : ws_session_base(
            srv, ag, ep)
        , ws_(std::move(stream))
    {
    }

    websocket::stream<
        stream_type>&
    ws()
    {
        return ws_;
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
};

//------------------------------------------------------------------------------

class ssl_ws_session_impl
    : public boost::enable_shared_from_this<ssl_ws_session_impl>
    , public ws_session_base<ssl_ws_session_impl>
{
    websocket::stream<
        beast::ssl_stream<
            stream_type>> ws_;

public:
    ssl_ws_session_impl(
        server& srv,
        agent& ag,
        beast::ssl_stream<
            stream_type> stream,
        endpoint_type ep)
        : ws_session_base(
            srv, ag, ep)
        , ws_(std::move(stream))
    {
    }

    websocket::stream<
        beast::ssl_stream<
            stream_type>>&
    ws()
    {
        return ws_;
    }

    // Report a failure
    void
    fail(beast::error_code ec, char const* what)
    {
        // ssl::error::ws_truncated, also known as an SSL "short read",
        // indicates the peer closed the connection without performing the
        // required closing handshake (for example, Google does this to
        // improve performance). Generally this can be a security issue,
        // but if your communication protocol is self-terminated (as
        // it is with both HTTP and WebSocket) then you may simply
        // ignore the lack of close_notify.
        //
        // https://github.com/boostorg/beast/issues/38
        //
        // https://security.stackexchange.com/questions/91435/how-to-handle-a-malicious-ssl-tls-shutdown
        //
        // When a short read would cut off the end of an HTTP message,
        // Beast returns the error beast::http::error::partial_message.
        // Therefore, if we see a short read here, it has occurred
        // after the message has been completed, so it is safe to ignore it.

        if(ec == asio::ssl::error::stream_truncated)
            return;

        if(ec == net::error::operation_aborted)
            LOG_TRC(log_, what, '\t', ec.message());
        else
            LOG_INF(log_, what, '\t', ec.message());
    }
};

} // (anon)

//------------------------------------------------------------------------------

void
run_ws_session(
    server& srv,
    agent& ag,
    stream_type stream,
    endpoint_type ep,
    websocket::request_type req)
{
    auto sp = boost::make_shared<
            plain_ws_session_impl>(
        srv,
        ag,
        std::move(stream),
        ep);
    sp->run(std::move(req));
}

void
run_ws_session(
    server& srv,
    agent& ag,
    beast::ssl_stream<
        stream_type> stream,
    endpoint_type ep,
    websocket::request_type req)
{
    auto sp = boost::make_shared<
            ssl_ws_session_impl>(
        srv,
        ag,
        std::move(stream),
        ep);
    sp->run(std::move(req));
}
