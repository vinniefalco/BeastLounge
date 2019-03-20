//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_RPC_IPP
#define BOOST_BEAST_JSON_IMPL_RPC_IPP

#include <boost/beast/_experimental/json/rpc.hpp>

namespace boost {
namespace beast {
namespace json {

namespace detail {

class rpc_error_codes : public error_category
{
public:
    const char*
    name() const noexcept override
    {
        return "boost.beast.json";
    }

    BOOST_BEAST_DECL
    std::string
    message(int ev) const override
    {
        switch(static_cast<rpc_error>(ev))
        {
        case rpc_error::parse_error: return
            "An error occurred on the server while parsing the JSON text.";
        case rpc_error::invalid_request: return
            "The JSON sent is not a valid Request object";
        case rpc_error::method_not_found: return
            "The method does not exist or is not available";
        case rpc_error::invalid_params: return
            "Invalid method parameters";
        case rpc_error::internal_error: return
            "Internal JSON-RPC error";

        case rpc_error::expected_object: return
            "Expected object in JSON-RPC request";
        case rpc_error::expected_string_version: return
            "Expected string version in JSON-RPC request";
        case rpc_error::unknown_version: return
            "Uknown version in JSON-RPC request";
        case rpc_error::invalid_null_id: return
            "Invalid null id in JSON-RPC request";
        case rpc_error::expected_strnum_id: return
            "Expected string or number id in JSON-RPC request";
        case rpc_error::expected_id: return
            "Missing id in JSON-RPC request version 1";
        case rpc_error::missing_method: return
            "Missing method in JSON-RPC request";
        case rpc_error::expected_string_method: return
            "Expected string method in JSON-RPC request";
        case rpc_error::expected_structured_params: return
            "Expected structured params in JSON-RPC request version 2";
        case rpc_error::missing_params: return
            "Missing params in JSON-RPC request version 1";
        case rpc_error::expected_array_params: return
            "Expected array params in JSON-RPC request version 1";
        }
        if( ev >= -32099 &&
            ev <= -32099)
            return "An implementation defined server error was received";
        return "Unknown RPC error #" + std::to_string(ev);
    }

    BOOST_BEAST_DECL
    error_condition
    default_error_condition(int ev) const noexcept override
    {
        return {ev, *this};
    }
};

} // detail

error_code
make_error_code(rpc_error e)
{
    static detail::rpc_error_codes const cat{};
    return error_code{static_cast<
        std::underlying_type<rpc_error>::type>(e), cat};
}

//------------------------------------------------------------------------------

rpc_request::
rpc_request(storage_ptr sp)
    : method(allocator<char>(sp))
    , params(sp)
    , id(std::move(sp))
{
}

void
rpc_request::
extract(value&& jv, error_code& ec)
{
    // clear the fields first
    method = string(allocator<char>(
        jv.get_storage()));
    params = value(kind::null,
        jv.get_storage());
    id = value(kind::null,
        jv.get_storage());

    // must be object
    if(! jv.is_object())
    {
        ec = rpc_error::expected_object;
        return ;
    }

    auto& obj = jv.as_object();

    // extract id first so on error,
    // the response can include it.
    {
        auto it = obj.find("id");
        if(it != obj.end())
            id.emplace(std::move(it->second));
    }

    // now check the version
    {
        auto it = obj.find("jsonrpc");
        if(it != obj.end())
        {
            if(! it->second.is_string())
            {
                ec = rpc_error::expected_string_version;
                return;
            }
            auto const& s =
                it->second.as_string();
            if(s != "2.0")
            {
                ec = rpc_error::unknown_version;
                return;
            }
            version = 2;
        }
        else
        {
            version = 1;
        }
    }

    // validate the extracted id
    {
        if(version == 2)
        {
            if(id.has_value())
            {
                // The use of Null as a value for the
                // id member in a Request is discouraged.
                if(id->is_null())
                {
                    ec = rpc_error::invalid_null_id;
                    return;
                }

                if( ! id->is_number() &&
                    ! id->is_string())
                {
                    ec = rpc_error::expected_strnum_id;
                    return;
                }
            }
        }
        else
        {
            // id must be present in 1.0
            if(! id.has_value())
            {
                ec = rpc_error::expected_id;
                return;
            }
        }
    }

    // extract method
    {
        auto it = obj.find("method");
        if(it == obj.end())
        {
            ec = rpc_error::missing_method;
            return;
        }
        if(! it->second.is_string())
        {
            ec = rpc_error::expected_string_method;
            return;
        }
        method = std::move(
            it->second.as_string());
    }

    // extract params
    {
        auto it = obj.find("params");
        if(version == 2)
        {
            if(it != obj.end())
            {
                if( ! it->second.is_object() &&
                    ! it->second.is_array())
                {
                    ec = rpc_error::expected_structured_params;
                    return;
                }
                params = std::move(it->second);
            }
        }
        else
        {
            if(it == obj.end())
            {
                ec = rpc_error::missing_params;
                return;
            }
            if(! it->second.is_array())
            {
                ec = rpc_error::expected_array_params;
                return;
            }
            params = std::move(it->second);
        }
    }
}

} // json
} // beast
} // boost

#endif
