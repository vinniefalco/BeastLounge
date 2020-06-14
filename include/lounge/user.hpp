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

#include <lounge/config.hpp>
#include <lounge/message.hpp>
#include <lounge/server.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <type_traits>

namespace lounge {

//class user_service;

class user
    : public boost::enable_shared_from
{
public:
    virtual ~user() = default;

    /** Send a message.
    */
    virtual
    void
    send(message m) = 0;

    /** Called when the associated connection is destroyed.
    */
    virtual
    void
    on_disconnect() = 0;

protected:
    friend class user_service;

    struct handler;
};

} // lounge

#endif
