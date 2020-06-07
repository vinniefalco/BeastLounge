//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_WS_CONNECTION_HPP
#define SRC_WS_CONNECTION_HPP

#include "any_connection.hpp"
#include "socket_type.hpp"
#include <lounge/server.hpp>
#include <boost/beast/websocket/rfc6455.hpp>

namespace lounge {

void
create_ws_connection(
    any_connection::list& list,
    server& srv,
    socket_type sock,
    beast::websocket::request_type req);

} // lounge

#endif
