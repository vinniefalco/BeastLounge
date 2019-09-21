//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "message.hpp"
#include <boost/beast/core/flat_static_buffer.hpp>
#include <boost/json/serializer.hpp>

//------------------------------------------------------------------------------

message
make_message(json::value const& jv)
{
    char buf[16384];
    json::serializer sr(jv);
    auto const n = sr.next(
        net::mutable_buffer(buf, sizeof(buf)));
    if(! sr.is_done())
    {
        // buffer overflow!
        return {};
    }
    return message(net::const_buffer(buf, n));
}

