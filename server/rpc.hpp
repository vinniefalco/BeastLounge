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

#include <boost/beast/_experimental/json/value.hpp>

inline
json::value
make_rpc_error(
    json::rpc_error ev,
    beast::string_view msg)
{
    json::value jv;
    jv["jsonrpc"] = "2.0";
    auto& err = jv["error"].emplace_object();
    err["code"] = static_cast<int>(ev);
    err["message"] = msg;
    jv["id"] = nullptr;
    return jv;
}

inline
json::value
make_rpc_error(
    json::rpc_error ev,
    beast::string_view msg,
    json::rpc_request const& req)
{
    json::value jv;
    jv["jsonrpc"] = "2.0";
    auto& err = jv["error"].emplace_object();
    err["code"] = static_cast<int>(ev);
    err["message"] = msg;
    if(req.id.has_value())
        jv["id"] = *req.id;
    return jv;
}

#endif
