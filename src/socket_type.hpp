//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_SOCKET_TYPE_HPP
#define SRC_SOCKET_TYPE_HPP

#include <lounge/config.hpp>
#include <lounge/executor_type.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace lounge {

using socket_type =
    net::basic_stream_socket<
        tcp, executor_type>;

} // lounge

#endif
