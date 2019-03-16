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
    : version(2)
    , method(allocator<char>(sp))
    , params(sp)
    , id(std::move(sp))
{
}

expected<rpc_request>
rpc_request::
extract(value&& jv)
{
    if(! jv.is_object())
        return error_code(
        rpc_error::invalid_request);
    auto& obj = jv.raw_object();

    rpc_request result(jv.get_storage());
    {
        auto it = obj.find("jsonrpc");
        if(it != obj.end())
        {
            if(! it->second.is_string())
                return error_code(
                    rpc_error::invalid_request);
            auto const& s =
                it->second.raw_string();
            if(s == "2.0")
                result.version = 2;
            else
                return error_code(
                    rpc_error::invalid_request);
        }
        else
        {
            result.version = 1;
        }
    }

    {
        auto it = obj.find("method");
        if(it == obj.end())
            return error_code(
                rpc_error::invalid_request);
        if(! it->second.is_string())
            return error_code(
                rpc_error::invalid_request);
        result.method = std::move(
            it->second.raw_string());
    }

    {
        auto it = obj.find("params");
        if(result.version == 2)
        {
            if(it != obj.end())
            {
                if( ! it->second.is_object() &&
                    ! it->second.is_array())
                return error_code(
                    rpc_error::invalid_request);
                result.params =
                    std::move(it->second);
            }
        }
        else
        {
            if(it == obj.end())
                return error_code(
                    rpc_error::invalid_request);
            if(! it->second.is_array())
                return error_code(
                    rpc_error::invalid_request);
            result.params = std::move(it->second);
        }
    }

    {
        auto it = obj.find("id");
        if(result.version == 2)
        {
            if(it != obj.end())
            {
                // The value SHOULD normally not be Null
                if( ! it->second.is_number() &&
                    ! it->second.is_string() &&
                    ! it->second.is_null())
                return error_code(
                    rpc_error::invalid_request);
                result.id = std::move(it->second);
            }
        }
        else
        {
            if(it == obj.end())
                return error_code(
                    rpc_error::invalid_request);
            result.id = std::move(it->second);
        }
    }

    return result;
}

} // json
} // beast
} // boost

#endif
