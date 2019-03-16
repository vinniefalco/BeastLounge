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
#include <boost/beast/_experimental/core/expected.hpp>
#include <cstddef>

namespace boost {
namespace beast {
namespace json {

/// Error codes returned by JSON operations
enum class rpc_error
{
    parse_error = -32700,

    invalid_request = -32600,

    method_not_found = -32601,

    invalid_params = -32602,

    internal_error = -32603
};

/** Represents a JSON-RPC request
*/
struct rpc_request
{
    /// Version of the request (1 or 2)
    int version;

    /// The request method
    string method;

    /** The request parameters

        This will be object, array, or null
    */
    value params;

    /** The request id

        This will be string, number, or null
    */
    value id;

    rpc_request() = default;

    BOOST_BEAST_DECL
    explicit
    rpc_request(storage_ptr sp);

    /** Extract a JSON-RPC request or return an error.
    */
    BOOST_BEAST_DECL
    static
    expected<rpc_request>
    extract(value&& jv);
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/rpc.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/rpc.ipp>
#endif

#endif
