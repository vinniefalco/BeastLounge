//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_USER_HPP
#define LOUNGE_USER_HPP

#include "config.hpp"
#include "session.hpp"
#include "utility.hpp"
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/container/flat_set.hpp>
#include <mutex>
#include <string>

class channel;
class message;

/// Represents a connected user
class user : public session
{
    std::mutex mutex_;
    boost::container::flat_set<channel*> channels_;

public:
    std::string name;

    ~user();

    void
    on_insert(channel& c);

    void
    on_erase(channel& c);

    /** Send a JSON message

        Messages are queued as needed.
        May be called from any thread.
    */
    virtual
    void
    send(json::value const& jv) = 0;

    /// Send a message
    virtual
    void
    send(message m) = 0;
};

#endif
