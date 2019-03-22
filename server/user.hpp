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
#include "message.hpp"
#include <boost/smart_ptr/weak_ptr.hpp>
#include <string>

/// Represents a connected user
class user
{
public:
    std::string name;

    virtual
    boost::weak_ptr<user>
    get_weak_ptr() = 0;

    /** Send a message
    */
    virtual
    void
    send(message m) = 0;
};

#endif
