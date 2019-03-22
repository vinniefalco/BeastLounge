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
#include <boost/beast/_experimental/json/value.hpp>
#include <stdexcept>

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

class rpc_exception
    : public std::exception
{
    int code_;
    std::string msg_;

public:
    rpc_exception()
        : rpc_exception(
            json::rpc_error::internal_error)
    {
    }

    rpc_exception(
        json::rpc_error ev)
        : rpc_exception(ev, 
            beast::error_code(ev).message())
    {
    }

    rpc_exception(
        beast::string_view msg)
        : rpc_exception(
            json::rpc_error::invalid_params,
            msg)
    {
    }

    rpc_exception(
        json::rpc_error ev,
        beast::string_view msg)
        : code_(static_cast<int>(ev))
        , msg_(msg)
    {
    }

    rpc_exception(
        beast::error_code const& ec)
        : code_(static_cast<int>(ec.value()))
        , msg_(ec.message())
    {
    }

    json::value
    to_json(
        boost::optional<json::value> const&
            id = boost::none) const
    {
        json::value jv;
        jv["jsonrpc"] = "2.0";
        auto& err = jv["error"].emplace_object();
        err["code"] = code_;
        err["message"] = msg_;
        if(id.has_value())
            jv["id"] = *id;
        return jv;
    }
};

inline
json::object&
checked_object(json::value& jv)
{
    if(! jv.is_object())
        throw rpc_exception{};
    return jv.as_object();
}

inline
json::array&
checked_array(json::value& jv)
{
    if(! jv.is_array())
        throw rpc_exception{};
    return jv.as_array();
}

inline
json::string&
checked_string(json::value& jv)
{
    if(! jv.is_string())
        throw rpc_exception{};
    return jv.as_string();
}

inline
json::number&
checked_number(json::value& jv)
{
    if(! jv.is_number())
        throw rpc_exception{};
    return jv.as_number();
}

inline
bool&
checked_bool(json::value& jv)
{
    if(! jv.is_bool())
        throw rpc_exception{};
    return jv.as_bool();
}

inline
void
checked_null(json::value& jv)
{
    if(! jv.is_null())
        throw rpc_exception{};
}

inline
json::value&
checked_value(
    json::value& jv,
    beast::string_view key)
{
    auto& obj =
        checked_object(jv);
    auto it = obj.find(key);
    if(it == obj.end())
        throw rpc_exception{};
    return it->second;
}

inline
json::object&
checked_object(
    json::value& jv,
    beast::string_view key)
{
    return checked_object(
        checked_value(jv, key));
}

inline
json::array&
checked_array(
    json::value& jv,
    beast::string_view key)
{
    return checked_array(
        checked_value(jv, key));
}

inline
json::string&
checked_string(
    json::value& jv,
    beast::string_view key)
{
    return checked_string(
        checked_value(jv, key));
}

inline
json::number&
checked_number(
    json::value& jv,
    beast::string_view key)
{
    return checked_number(
        checked_value(jv, key));
}

inline
bool&
checked_bool(
    json::value& jv,
    beast::string_view key)
{
    return checked_bool(
        checked_value(jv, key));
}

inline
void
checked_null(
    json::value& jv,
    beast::string_view key)
{
    checked_null(checked_value(jv, key));
}

#endif
