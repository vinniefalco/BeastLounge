//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_WS_SESSION_HPP
#define LOUNGE_WS_SESSION_HPP

#include "config.hpp"
#include "message.hpp"
#include "session.hpp"
#include <boost/smart_ptr/weak_ptr.hpp>

class ws_session : public session
{
public:
    virtual
    boost::weak_ptr<ws_session>
    get_weak_ptr() = 0;

    /** Send a message
    */
    virtual
    void
    send(message m) = 0;
};

#endif
