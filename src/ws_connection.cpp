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
#include <boost/beast/websocket/stream.hpp>

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
        , log_(srv.get_log("ws_connection"))
    {
    }

    void
    run()
    {
        net::post(
            derived().stream().get_executor(),
            beast::bind_front_handler(
                &connection::do_accept,
                boost::shared_from(this)));
    }

    void
    do_accept()
    {
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
    socket_type sock)
{
    auto sp = emplace_any_connection<
        plain_ws>(
            list,
            srv,
            std::move(sock));
    if(sp)
        sp->run();
}

} // lounge
