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
#include <boost/json/value.hpp>
#include <boost/optional.hpp>
#include <stdexcept>
#include <utility>

namespace lounge {

class rpc_request
{
public:
};

class rpc_response
{
public:
    explicit
    rpc_response(
        json::storage_ptr sp = {}) noexcept
    {
    }
};

#if 0
/** Represents a JSON-RPC request
*/
class rpc_request
{
    /** The request id

        If set, this will be string, number, or null
    */
    boost::optional<json::value> id_;

public:
    /// Version of the request (1 or 2)
    int version = 2;

    /// The request method
    json::string method;

    /** The request parameters

        This will be object, array, or null
    */
    json::value params;

public:
    request(rpc_request&&) = default;
    request& rpc_request=(rpc_request&&) = delete;

    /** Construct an empty request using the specified storage.

        The method, params, and id will be null,
        and version will be 2.
    */
    explicit
    request(
        json::storage_ptr sp = {});

    /** Extract a JSON-RPC request or return an error.
    */
    void
    extract(
        json::value&& jv,
        error_code& ec);
};
#endif

} // lounge

#endif
