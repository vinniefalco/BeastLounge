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
#include <boost/beast/_experimental/json/value.hpp>
#include <memory>

class user;

class channel
{
public:
    virtual ~channel() = default;

    /** Add a user to the channel.

        @returns `false` if the user was already in the channel.
    */
    virtual
    bool
    insert(user& u) = 0;

    virtual
    void
    erase(user& u) = 0;

    virtual
    void
    send(json::value const& jv) = 0;
};

extern
std::unique_ptr<channel>
make_channel();

#endif
