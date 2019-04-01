//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "channel_list.hpp"
#include "message.hpp"
#include "rpc.hpp"
#include "user.hpp"
#include <atomic>

channel::
channel(
    beast::string_view name,
    channel_list& list)
    : list_(list)
    , uid_(list.next_uid())
    , cid_(list.next_cid())
    , name_(name)
{
}

channel::
channel(
    std::size_t reserved_cid,
    beast::string_view name,
    channel_list& list)
    : list_(list)
    , uid_(list.next_uid())
    , cid_(reserved_cid)
    , name_(name)
{
}

channel::
~channel()
{
    // The proper way to delete a channel is
    // to first remove all the users, so they
    // get the notification.
    BOOST_ASSERT(users_.empty());

    list_.erase(*this);
}

bool
channel::
is_joined(user& u) const noexcept
{
    lock_type lock(mutex_);
    return users_.find(&u) != users_.end();
}

bool
channel::
insert(user& u)
{
    {
        bool inserted;
        {
            lock_type lock(mutex_);
            inserted = users_.insert(&u).second;
        }
        if(! inserted)
            return false;
    }
    {
        // broadcast: join
        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "join";
        jv["name"] = name();
        jv["user"] = u.name;
        send(jv);
    }
    u.on_insert(*this);
    on_insert(u);
    return true;
}

bool
channel::
erase(user& u)
{
    {
        lock_type lock(mutex_);
        if(users_.erase(&u) == 0)
            return false;
    }
    {
        // broadcast: leave
        json::value jv;
        jv["cid"] = cid();
        jv["verb"] = "leave";
        jv["name"] = name();
        jv["user"] = u.name;
        send(jv);
    }
    u.on_erase(*this);
    on_erase(u);
    return true;
}

void
channel::
send(json::value const& jv)
{
    send(make_message(jv));
}

void
channel::
dispatch(rpc_call& rpc)
{
    if(rpc.method == "join")
    {
        do_join(rpc);
    }
    else if(rpc.method == "leave")
    {
        do_leave(rpc);
    }
    else
    {
        on_dispatch(rpc);
    }
}

void
channel::
checked_user(rpc_call& rpc)
{
    if(rpc.user.name.empty())
        rpc.fail("No identity set");
}

void
channel::
do_join(rpc_call& rpc)
{
    checked_user(rpc);
    if(! insert(rpc.user))
        rpc.fail("Already in channel");
    rpc.respond();
}

void
channel::
do_leave(rpc_call& rpc)
{
    if(! erase(rpc.user))
        rpc.fail("Not in channel");
    rpc.respond();
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
        lock_type lock(mutex_);
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
}
