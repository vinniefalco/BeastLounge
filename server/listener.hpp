//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_LISTENER_HPP
#define LOUNGE_LISTENER_HPP

#include "config.hpp"
#include "server.hpp"
#include <boost/beast/core/error.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>
#include <string>

//------------------------------------------------------------------------------

/** Configuration for a listening socket.
*/
struct listener_config
{
    // name of this port for logs
    std::string name;

    // endpoint to bind to
    net::ip::address address;

    // port number
    unsigned short port_num;

    enum
    {
        no_tls,
        allow_tls,
        require_tls
    } kind;
};

/** Create a listening socket to accept connections.

    @returns `true` on success
*/
extern
bool
make_listener(
    server& srv,
    listener_config cfg);

#endif
