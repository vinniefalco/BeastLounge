//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "sigint_service.hpp"
#include <boost/beast/core/bind_handler.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio/basic_signal_set.hpp>
#include <boost/asio/post.hpp>

namespace lounge {

namespace {

class sigint_service_impl
    : public sigint_service
{
    server& srv_;
    asio::basic_signal_set<
        executor_type> signals_;

public:
    using key_type = sigint_service;

    sigint_service_impl(
        server& srv)
        : srv_(srv)
        , signals_(
            srv.make_executor(),
            SIGINT,
            SIGTERM)
    {
    }

    ~sigint_service_impl()
    {
    }

    void
    on_start() override
    {
        // Capture SIGINT and SIGTERM
        signals_.async_wait(
            beast::bind_front_handler(
                &sigint_service_impl::on_signal,
                this));
    }

    void
    on_signal(
        beast::error_code ec,
        int /*signum*/)
    {
        if(ec == net::error::operation_aborted)
            return;

        // log

        srv_.stop();
    }

    void
    on_stop() override
    {
        net::post(
            signals_.get_executor(),
            beast::bind_front_handler(
                &sigint_service_impl::do_stop,
                this));
    }

    void
    do_stop()
    {
        beast::error_code ec;
        signals_.cancel(ec);
    }
};

} // (anon)

void
sigint_service::
install(
    server& srv)
{
    emplace_service<sigint_service_impl>(srv);
}

} // lounge
