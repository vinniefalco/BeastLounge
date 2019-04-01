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
#include "uid.hpp"
#include "utility.hpp"
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/container/flat_set.hpp>
#include <mutex>
#include <vector>

class channel_list;
class message;
class rpc_call;
class user;

//------------------------------------------------------------------------------

class channel : public enable_shared_from
{
    using lock_type =
        std::lock_guard<std::mutex>;

    channel_list& list_;
    std::mutex mutable mutex_;
    boost::container::flat_set<user*> users_;
    uid_type uid_;
    std::size_t cid_;
    std::string name_;

    friend channel_list;

public:
    virtual
    ~channel();

    /// Return the channel unique-id
    uid_type
    uid() const noexcept
    {
        return uid_;
    }

    /// Return the channel id
    std::size_t
    cid() const noexcept
    {
        return cid_;
    }

    /// Return the channel name
    beast::string_view
    name() const noexcept
    {
        return name_;
    }

    /// Returns `true` if the user has joined the channel
    bool
    is_joined(user& u) const noexcept;

    /** Add a user to the channel.

        @returns `false` if the user was already in the channel.
    */
    bool
    insert(user& u);

    bool
    erase(user& u);

    void
    send(json::value const& jv);

    /// Process an RPC command for this channel
    void
    dispatch(rpc_call& rpc);

protected:
    /// Construct a new channel with a unique channel id
    channel(
        beast::string_view name,
        channel_list& list);

    /// Construct a new channel with the specified channel id
    channel(
        std::size_t reserved_cid,
        beast::string_view name,
        channel_list& list);

    void
    checked_user(rpc_call& rpc);

    virtual
    void
    on_insert(user& u) = 0;

    virtual
    void
    on_erase(user& u) = 0;

    /// Called on an RPC command
    virtual
    void
    on_dispatch(rpc_call& rpc) = 0;

private:
    void do_join(rpc_call& rpc);
    void do_leave(rpc_call& rpc);
    void send(message m);
};

#endif
