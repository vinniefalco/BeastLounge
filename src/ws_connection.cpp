//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "ws_connection.hpp"
#include "http_service.hpp"
#include <lounge/user.hpp>
#include <lounge/user_service.hpp>
#include <boost/beast/core/buffers_to_string.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <utility>
#include <vector>

#include <lounge/chat_service.hpp>

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
    beast::flat_buffer buf_;
    std::vector<message> mq_;
    boost::shared_ptr<user> user_;

    Derived&
    derived() noexcept
    {
        return static_cast<
            Derived&>(*this);
    }

    struct user_handler
    {
        boost::weak_ptr<
            connection> wp;

        void
        do_send(message m)
        {
            auto sp = wp.lock();
            if(sp)
                sp->send(std::move(m));
        }
    };

public:
    connection(
        server& srv)
        : srv_(srv)
        , svc_(get_service<http_service>(srv))
        , log_(srv.get_log("ws_connection"))
    {
    }

    ~connection()
    {
        user_->on_disconnect();
    }

    void
    run(beast::websocket::request_type req)
    {
        net::post(
            derived().stream().get_executor(),
            beast::bind_front_handler(
                &connection::do_accept,
                boost::shared_from(this),
                std::move(req)));
    }

    void
    do_accept(
        beast::websocket::request_type req)
    {
        derived().stream().async_accept(
            std::move(req),
            beast::bind_front_handler(
                &connection::on_accept,
                boost::shared_from(this)));
    }

    void
    on_accept(error_code ec)
    {
        if(ec)
        {
            LOG_ERR(log_, "on_accept: ", ec.message());
            return;
        }

        // can't use weak_from in ctor
        user_ = get_service<
            user_service>(srv_).create_user(
                boost::weak_from(this),
                user_handler{
                    boost::weak_from(this)});
        get_service<chat_service>(srv_).insert(*user_);

        do_read();
    }

    void
    do_read()
    {
        buf_.clear();
        derived().stream().async_read(
            buf_,
            beast::bind_front_handler(
                &connection::on_read,
                boost::shared_from(this)));
    }

    void
    on_read(error_code ec, std::size_t)
    {
        if(ec)
        {
            LOG_ERR(log_, "on_read: ", ec.message());
            return;
        }

        // process message
        LOG_INF(log_, "on_read: ",
            beast::buffers_to_string(
                buf_.data()));
        std::string const s =
            beast::buffers_to_string(
                buf_.data());

get_service<chat_service>(srv_).on_msg(s);

        do_read();
    }

    void
    send(message m)
    {
        net::dispatch(
            derived().stream().get_executor(),
            beast::bind_front_handler(
                &connection::do_send,
                boost::shared_from(this),
                std::move(m)));
    }

    void
    do_send(message m)
    {
        if(! beast::get_lowest_layer(
            derived().stream()).is_open())
            return;
        mq_.emplace_back(std::move(m));
        if(mq_.size() == 1)
            do_write();
    }

    void
    do_write()
    {
        BOOST_ASSERT(! mq_.empty());

        derived().stream().async_write(
            mq_.back(),
            beast::bind_front_handler(
                &connection::on_write,
                boost::shared_from(this),
                mq_.size() - 1));
    }

    void
    on_write(
        std::size_t idx,
        beast::error_code ec,
        std::size_t)
    {
        BOOST_ASSERT(! mq_.empty());
        if(ec)
        {
            LOG_INF(log_, "on_write: ", ec.message());
            return;
        }
        auto const last = mq_.size() - 1;
        if(idx != last)
            swap(mq_[idx], mq_[last]);
        mq_.resize(last);
        if(! mq_.empty())
            do_write();
    }
};

//----------------------------------------------------------

class plain_ws
    : public connection<plain_ws>
{
    beast::websocket::stream<
        net::basic_stream_socket<
            tcp, executor_type>> stream_;

public:
    plain_ws(
        server& srv,
        socket_type sock)
        : connection<plain_ws>(srv)
        , stream_(std::move(sock))
    {
    }

    ~plain_ws()
    {
    }

    beast::websocket::stream<
        net::basic_stream_socket<
            tcp, executor_type>>&
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
                &plain_ws::do_close,
                boost::shared_from(this)));
    }

    void
    do_close()
    {
        //error_code ec;
        //stream_.async_close(
    }
};

//----------------------------------------------------------

} // (anon)

void
create_ws_connection(
    any_connection::list& list,
    server& srv,
    socket_type sock,
    beast::websocket::request_type req)
{
    auto sp = emplace_any_connection<
        plain_ws>(
            list,
            srv,
            std::move(sock));
    if(sp)
        sp->run(std::move(req));
}

} // lounge
