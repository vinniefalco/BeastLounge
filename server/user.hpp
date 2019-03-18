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
#include "types.hpp"

class user
{
    tcp::endpoint ep_;

public:
    user(tcp::endpoint const& ep);

    tcp::endpoint const&
    endpoint() const noexcept
    {
        return ep_;
    }
};

#endif
