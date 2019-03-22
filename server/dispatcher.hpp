//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_DISPATCHER_HPP
#define LOUNGE_DISPATCHER_HPP

#include "config.hpp"
#include <boost/beast/core/string.hpp>
#include <boost/beast/_experimental/json/rpc.hpp>
#include <boost/asio/buffer.hpp>
#include <functional>

class message;
class user;

/// Processes RPC commands
class dispatcher
{
public:
    using handler_type =
        std::function<void(
            user&, json::rpc_request&)>;

    virtual ~dispatcher() = default;

    virtual
    void
    insert(
        beast::string_view method,
        handler_type handler) = 0;

    template<class T>
    void
    insert(
        beast::string_view method,
        void (T::*mf)(user&, json::rpc_request&),
        T* t)
    {
        namespace ph = std::placeholders;
        insert(method, std::bind(
            mf, t, ph::_1, ph::_2));
    }

    virtual
    void
    dispatch(
        user& u, net::const_buffer b) = 0;
};

#endif
