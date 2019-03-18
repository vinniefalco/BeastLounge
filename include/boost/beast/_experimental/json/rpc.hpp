 //
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_RPC_HPP
#define BOOST_BEAST_JSON_RPC_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/optional.hpp>
#include <cstddef>

namespace boost {
namespace beast {
namespace json {

/// Error codes returned by JSON operations
enum class rpc_error
{
    parse_error      = -32700,
    invalid_request  = -32600,
    method_not_found = -32601,
    invalid_params   = -32602,
    internal_error   = -32603,

    /// Expected object in JSON-RPC request
    expected_object = 1,

    /// Expected string version in JSON-RPC request
    expected_string_version,

    /// Uknown version in JSON-RPC request
    unknown_version,

    /// Invalid null id in JSON-RPC request
    invalid_null_id,

    /// Expected string or number id in JSON-RPC request
    expected_strnum_id,

    /// Missing id in JSON-RPC request version 1
    expected_id,

    /// Missing method in JSON-RPC request
    missing_method,

    /// Expected string method in JSON-RPC request
    expected_string_method,

    /// Expected structured params in JSON-RPC request version 2
    expected_structured_params,

    /// Missing params in JSON-RPC request version 1
    missing_params,

    /// Expected array params in JSON-RPC request version 1
    expected_array_params
};

//------------------------------------------------------------------------------

/** Represents a JSON-RPC request
*/
struct rpc_request
{
    /// Version of the request (1 or 2)
    int version = 2;

    /// The request method
    string method;

    /** The request parameters

        This will be object, array, or null
    */
    value params;

    /** The request id

        If set, this will be string, number, or null
    */
    boost::optional<value> id;

    /** Construct an empty request using the default storage.

        The method, params, and id will be null,
        and version will be 2.
    */
    rpc_request() = default;

    /** Construct an empty request using the specified storage.

        The method, params, and id will be null,
        and version will be 2.
    */
    BOOST_BEAST_DECL
    explicit
    rpc_request(storage_ptr sp);

    /** Extract a JSON-RPC request or return an error.
    */
    BOOST_BEAST_DECL
    void
    extract(value&& jv, error_code& ec);
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/rpc.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/rpc.ipp>
#endif

#endif
