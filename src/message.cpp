//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <lounge/message.hpp>
#include <boost/json/serializer.hpp>

namespace lounge {

message::
message(json::value const& jv)
{
    char buf[8192];
    json::serializer sr(jv);
    auto n = sr.read(buf, sizeof(buf));
    if(sr.is_done())
    {
        p_ = construct(
            net::const_buffer(buf, n));
    }
    else
    {
        json::string s;
        s.append({ buf, n });
        do
        {
            if(s.size() >= s.capacity())
                s.reserve(s.capacity() + 1);
            s.grow(static_cast<
                json::string::size_type>(
                sr.read(s.data() + s.size(),
                    s.capacity() - s.size())));
        }
        while(! sr.is_done());
        p_ = construct(net::const_buffer(
            s.data(), s.size()));
    }
}

} // lounge
