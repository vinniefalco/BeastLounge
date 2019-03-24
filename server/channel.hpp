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
#include <boost/container/flat_set.hpp>
#include <mutex>
#include <vector>

class message;
class user;

//------------------------------------------------------------------------------

class channel
{
    std::mutex mutex_;
    boost::container::flat_set<user*> users_;
    std::size_t cid_;
    std::string name_;

public:
    ~channel();

    /// Construct a new channel with a unique channel id
    explicit
    channel(beast::string_view name);

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

    /** Add a user to the channel.

        @returns `false` if the user was already in the channel.
    */
    bool
    insert(user& u);

    void
    erase(user& u);

    void
    send(json::value const& jv);

protected:
    virtual
    void
    on_insert(user& u) = 0;

    virtual
    void
    on_erase(user& u) = 0;

private:
    void
    send(message m);
};

#endif
