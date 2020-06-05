//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_RPC_HPP
#define LOUNGE_RPC_HPP

#include <lounge/config.hpp>
#include <boost/json/storage_ptr.hpp>
//#include <boost/json/value.hpp>

namespace lounge {

class rpc_response
{
public:
    explicit
    rpc_response(
        json::storage_ptr sp = {}) noexcept
    {
    }
};

} // lounge

#endif
