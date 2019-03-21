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

#include "config.hpp"
#include "ws_session.hpp"
#include <boost/beast/_experimental/json/rpc.hpp>

class rpc_handler
{
public:
    virtual ~rpc_handler() = default;

    virtual
    void
    on_request(
        ws_session& user,
        json::rpc_request&& req) = 0;
};

#endif
