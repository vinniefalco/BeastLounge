//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "user.hpp"

//------------------------------------------------------------------------------

user::
user(tcp::endpoint const& ep)
    : ep_(ep)
{
}

//------------------------------------------------------------------------------

namespace {

class user_impl : public user
{
public:
    explicit
    user_impl(
        tcp::endpoint const& ep)
        : user(ep)
    {
    }
};

} // (anon)

//------------------------------------------------------------------------------

boost::shared_ptr<user>
make_user(
    tcp::endpoint const& ep)
{
    return boost::make_shared<
        user_impl>(
            ep);
}
