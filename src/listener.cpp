//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "listener.hpp"
#include "any_connection.hpp"
#include <boost/beast/core/bind_handler.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/make_unique.hpp>

namespace lounge {

namespace {

class listener_impl
    : public listener
{
    server& srv_;
    handler& h_;
    log& log_;

    asio::ssl::context ctx_;

    tcp::endpoint local_ep_;
    tcp::endpoint remote_ep_;
    net::basic_socket_acceptor<
        tcp, executor_type> acceptor_;

    std::unique_ptr<
        any_connection::list> list_;

public:
    listener_impl(
        server& srv,
        handler& h,
        tcp::endpoint ep)
        : srv_(srv)
        , h_(h)
        , log_(srv.get_log("listener"))
        , ctx_(asio::ssl::context::tlsv12)
        , local_ep_(ep)
        , acceptor_(
            srv.make_executor())
        , list_(any_connection::list::create())
    {
    }

    //------------------------------------------------------

    void
    on_start() override
    {
        if(! open())
            return;

        acceptor_.async_accept(
            srv_.make_executor(),
            remote_ep_,
            beast::bind_front_handler(
                &listener_impl::do_accept,
                this));
    }

    void
    on_stop() override
    {
        LOG_TRC(log_, "on_stop");

        list_->close();

        net::post(
            acceptor_.get_executor(),
            beast::bind_front_handler(
                &listener_impl::do_stop,
                this));
    }

    //------------------------------------------------------

    bool
    open()
    {
        error_code ec;

        // Open the acceptor
        acceptor_.open(local_ep_.protocol(), ec);
        if(ec)
        {
            LOG_DBG(log_,
                "open: ", ec.message());
            return false;
        }

        // Allow address reuse
        acceptor_.set_option(
            net::socket_base::reuse_address(true));
        if(ec)
        {
            LOG_DBG(log_,
                "set_option: ", ec.message());
            return false;
        }

        // Bind to the server address
        acceptor_.bind(local_ep_, ec);
        if(ec)
        {
           LOG_DBG(log_,
                "bind: ", ec.message());
            return false;
        }

        // Start listening for connections
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec)
        {
           LOG_DBG(log_,
                "listen: ", ec.message());
            return false;
        }

        return true;
    }

    void
    do_stop()
    {
        error_code ec;
        acceptor_.close(ec);
    }

    void
    do_accept(
        error_code ec,
        socket_type sock)
    {
        // Report the error, if any
        if(ec)
        {
            if(ec == net::error::operation_aborted)
            {
                LOG_TRC(log_,
                    "do_accept: ", ec.message());
                return;
            }
            LOG_INF(log_,
                "do_accept: ", ec.message());
            return;
        }

        // Check stop conditions
        if( ! list_->is_open() ||
            ! acceptor_.is_open())
            return;

        h_.on_accept(std::move(sock));

        // Accept the next connection
        acceptor_.async_accept(
            srv_.make_executor(),
            remote_ep_,
            beast::bind_front_handler(
                &listener_impl::do_accept,
                this));
    }
};

} // (anon)

std::unique_ptr<listener>
listener::
create(
    server& srv,
    handler& h,
    tcp::endpoint ep)
{
    return boost::make_unique<
        listener_impl>(srv, h, ep);
}

} // lounge
