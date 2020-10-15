//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CHAT_SERVICE_HPP
#define LOUNGE_CHAT_SERVICE_HPP

#include <lounge/server.hpp>
#include <memory>

namespace lounge {

class user;

class chat_service
    : public service
{
public:
    virtual
    void
    insert(user& u) = 0;

    virtual
    void
    on_msg(string_view s) = 0;

    static
    chat_service&
    install(
        server& srv);
};

} // lounge

#endif