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
    shared_lock_guard lock(mutex_);
    return users_.find(&u) != users_.end();
}

bool
channel::
insert(user& u)
{
    {
        auto const inserted = [&]
        {
            lock_guard lock(mutex_);
            return users_.insert(&u).second;
        }();
        if(! inserted)
            return false;
    }
    {
        // broadcast: join
        json::value jv(json::object_kind);
        auto& obj = jv.get_object();
        obj["cid"] = cid();
        obj["verb"] = "join";
        obj["name"] = name();
        obj["user"] = u.name;
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
    // First remove the user from the list
    {
        lock_guard lock(mutex_);
        if(users_.erase(&u) == 0)
            return false;
    }

    // Notify channel participants
    {
        // broadcast: leave
        json::value jv(json::object_kind);
        auto& obj = jv.get_object();
        obj["cid"] = cid();
        obj["verb"] = "leave";
        obj["name"] = name();
        obj["user"] = u.name;
        send(jv);

        // Also notify the user, if
        // they are still connected.

        if(auto sp = boost::weak_from(&u).lock())
            sp->send(jv);
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
    if(rpc.u->name.empty())
        rpc.fail("No identity set");
}

void
channel::
do_join(rpc_call& rpc)
{
    checked_user(rpc);
    if(! insert(*rpc.u))
        rpc.fail("Already in channel");
    rpc.complete();
}

void
channel::
do_leave(rpc_call& rpc)
{
    if(! erase(*rpc.u))
        rpc.fail("Not in channel");
    rpc.complete();
}

void
channel::
send(message m)
{
    // Make a local list of all the weak pointers
    // representing the users, so we can do the
    // actual sending without holding the mutex:
    std::vector<boost::weak_ptr<user>> v;
    {
        shared_lock_guard lock(mutex_);
        v.reserve(users_.size());
        for(auto p : users_)
            v.emplace_back(boost::weak_from(p));
    }

    // For each user in our local list, try to
    // acquire a strong pointer. If successful,
    // then send the message to that user.
    for(auto const& wp : v)
        if(auto sp = wp.lock())
            sp->send(m);
}
