//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "system_channel.hpp"
#include "user.hpp"

//------------------------------------------------------------------------------

system_channel::
system_channel()
    : channel("System")
{
}

void
system_channel::
on_insert(user& u)
{
    json::value jv;
    jv["channel"] = cid();
    jv["verb"] = "join";
    jv["user"] = u.name;
    u.send(jv);
}

void
system_channel::
on_erase(user& u)
{
    boost::ignore_unused(u);
}
