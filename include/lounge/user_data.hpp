//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_USER_DATA_HPP
#define LOUNGE_USER_DATA_HPP

#include <lounge/config.hpp>

namespace lounge {

class user;

namespace detail {

class user_data_base
{
protected:
    friend class user_service;

    struct handler;
};

} // detail

template<class T>
class user_data
    : public user_data_base
{
public:
};

} // lounge

#endif
