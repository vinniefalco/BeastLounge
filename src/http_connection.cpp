//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "http_connection.hpp"
#include "buffers_generator.hpp"
#include "http_generator.hpp"
#include "http_service.hpp"
#include "ws_connection.hpp"

#include <boost/beast/core/basic_stream.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/stream_traits.hpp>
#include <boost/beast/http/parser.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/websocket/rfc6455.hpp>

namespace lounge {

namespace {

//----------------------------------------------------------

template<class Derived>
class connection
    : public any_connection
{
    server& srv_;
    http_service& svc_;
    log& log_;

    beast::flat_buffer buffer_;
    boost::optional<
        http::request_parser<
            http::string_body>> pr_;

    Derived&
    derived() noexcept
    {
        return static_cast<
            Derived&>(*this);
    }

public:
    connection(
        server& srv)
        : srv_(srv)
        , svc_(get_service<http_service>(srv))
        , log_(srv.get_log("http_connection"))
    {
    }

    void
    run()
    {
        net::post(
            derived().stream().get_executor(),
            beast::bind_front_handler(
                &connection::do_read,
                boost::shared_from(this)));
    }

    void
    do_read()
    {
        // A newly constructed HTTP parser
        // is required for each message.
        pr_.emplace();

        // Set the timeout
        beast::get_lowest_layer(
            derived().stream()).expires_after(
                std::chrono::seconds(30));

        // Set some limits to discourage abuse.
        pr_->body_limit(64 * 1024);
        pr_->header_limit(2048);

        // Read the next HTTP request
        http::async_read(
            derived().stream(),
            buffer_,
            *pr_,
            beast::bind_front_handler(
                &connection::on_read,
                boost::shared_from(this)));
    }

    void
    on_read(
        error_code ec,
        std::size_t bytes_transferred)
    {
        (void)bytes_transferred;

        // This means they closed the connection
        if(ec == http::error::end_of_stream)
        {
// VFALCO FIX
            //return impl()->do_close();
            return;
        }

        // This happens when I/O is canceled
        if(ec == net::error::operation_aborted)
        {
            LOG_TRC(log_,
                "on_read: ", ec.message());
            return;
        }

        // This is an actual error
        if(ec)
        {
            LOG_INF(log_,
                "on_read: ", ec.message());
            return;
        }

        // See if it is a WebSocket Upgrade
        if(websocket::is_upgrade(pr_->get()))
        {
            // Turn off the timeout
            beast::get_lowest_layer(
                derived().stream()).expires_never();

            // Convert the request type
            websocket::request_type req(
                pr_->release());

            // Create a WebSocket session
            create_ws_connection(
                this->get_list(),
                srv_,
                std::move(derived().stream().release_socket()),
                std::move(req));
            return;
        }

        // Calculate response and send it
        auto res = svc_.handle_request(
            pr_->release());
        async_write(
            derived().stream(),
            res,
            beast::bind_front_handler(
                &connection::on_write,
                boost::shared_from(this),
                res.keep_alive()));
    }

    void
    on_write(
        bool keep_alive,
        error_code ec,
        std::size_t bytes_transferred)
    {
        (void)bytes_transferred;

        // This means they closed the connection
        if(ec == http::error::end_of_stream)
        {
// VFALCO FIX
            //return impl()->do_close();
            return;
        }

        // This happens when I/O is canceled
        if(ec == net::error::operation_aborted)
        {
            LOG_TRC(log_,
                "on_write: ", ec.message());
            return;
        }

        // This is an actual error
        if(ec)
        {
            LOG_INF(log_,
                "on_write: ", ec.message());
            return;
        }

        // Read the next request
        if(keep_alive)
            do_read();
    }
};

//----------------------------------------------------------

class plain_http
    : public connection<plain_http>
{
    beast::basic_stream<
        tcp, executor_type> stream_;

public:
    plain_http(
        server& srv,
        socket_type sock)
        : connection<plain_http>(srv)
        , stream_(std::move(sock))
    {
    }

    ~plain_http()
    {
    }

    beast::basic_stream<
        tcp, executor_type>&
    stream()
    {
        return stream_;
    }

    void
    close() override
    {
        net::post(
            stream_.get_executor(),
            beast::bind_front_handler(
                &plain_http::do_close,
                boost::shared_from(this)));
    }

    void
    do_close()
    {
        error_code ec;
        stream_.cancel();
    }
};

//----------------------------------------------------------

} // (anon)

void
create_http_connection(
    any_connection::list& list,
    server& srv,
    socket_type sock)
{
    auto sp = emplace_any_connection<
        plain_http>(
            list,
            srv,
            std::move(sock));
    if(sp)
        sp->run();
}

} // lounge
