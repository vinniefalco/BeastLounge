//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "user.hpp"
#include "channel.hpp"
#include <boost/assert.hpp>

user::
~user()
{
    for(auto c : channels_)
        c->erase(*this);
}

void
user::
on_insert(channel& c)
{
    std::lock_guard<std::mutex> lock(mutex_);
    BOOST_VERIFY(channels_.insert(&c).second);
}

void
user::
on_erase(channel& c)
{
    std::lock_guard<std::mutex> lock(mutex_);
    BOOST_VERIFY(channels_.erase(&c) == 1);
}
