//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CHANNEL_HPP
#define LOUNGE_CHANNEL_HPP

#include "config.hpp"
#include <memory>

class message;
class user;

class channel
{
public:
    virtual ~channel() = default;

    virtual
    void
    insert(user*) = 0;

    virtual
    void
    erase(user*) = 0;

    virtual
    void
    send(message m) = 0;
};

extern
std::unique_ptr<channel>
make_channel();

#endif
