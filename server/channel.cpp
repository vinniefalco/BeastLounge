//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "message.hpp"
#include "user.hpp"
#include <atomic>

channel::
channel()
    : cid_(
        []
        {
            static std::atomic<std::size_t> cid = 0;
            return ++cid;
        }())
{
}

channel::
~channel()
{
    // The proper way to delete a channel is
    // to first remove all the users, so they
    // get the notification.
    BOOST_ASSERT(users_.empty());
}

bool
channel::
insert(user& u)
{
    if(![&]
        {
            std::lock_guard<std::mutex> lock(mutex_);
            return users_.insert(&u).second;
        }())
        return false;
    u.on_insert(*this);
    on_insert(u);
    return true;
}

void
channel::
erase(user& u)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        users_.erase(&u);
    }
    u.on_erase(*this);
    on_erase(u);
}

void
channel::
send(json::value const& jv)
{
    send(make_message(jv));
}

void
channel::
send(message m)
{
    // Make a local list of all the weak pointers
    // representing the sessions, so we can do the
    // actual sending without holding the mutex:
    std::vector<boost::weak_ptr<user>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(users_.size());
        for(auto p : users_)
            v.emplace_back(weak_from(p));
    }

    // For each user in our local list, try to
    // acquire a strong pointer. If successful,
    // then send the message to that user.
    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->send(m);
};
