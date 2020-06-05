//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_LISTENER_HPP
#define SRC_LISTENER_HPP

#include "socket_type.hpp"
#include <lounge/server.hpp>
#include <boost/asio/basic_stream_socket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <memory>

namespace lounge {

// A listening socket and configuration
class listener
{
public:
    struct handler
    {
        virtual
        void
        on_accept(
            socket_type) = 0;
    };

    virtual ~listener() = default;

    virtual
    void
    on_start() = 0;

    virtual
    void
    on_stop() = 0;

    static
    std::unique_ptr<listener>
    create(
        server& srv,
        handler& h,
        tcp::endpoint ep);
};

} // lounge

#endif
