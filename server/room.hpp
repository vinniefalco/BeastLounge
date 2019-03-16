//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_ROOM_HPP
#define LOUNGE_ROOM_HPP

#include "config.hpp"
#include "message.hpp"
#include "types.hpp"
#include "ws_session.hpp"
#include <boost/smart_ptr/shared_ptr.hpp>

class room
{
public:
    virtual ~room() = default;

    virtual
    void
    insert(ws_session*) = 0;

    virtual
    void
    erase(ws_session*) = 0;

    virtual
    void
    send(message m) = 0;
};

extern
boost::shared_ptr<room>
make_room();

#endif
