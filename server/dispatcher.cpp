//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "dispatcher.hpp"
#include "message.hpp"
#include "rpc.hpp"
#include "server.hpp"
#include "user.hpp"
#include <boost/beast/_experimental/json/parser.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/make_unique.hpp>

namespace {

class dispatcher_impl
    : public dispatcher
{
    struct command
    {
        beast::string_view method;
        handler_type handler;

        command(
            beast::string_view method_,
            handler_type&& handler_)
            : method(method_)
            , handler(std::move(handler_))
        {
        }

        operator
        beast::string_view() const noexcept
        {
            return method;
        }
    };

    struct compare
    {
        using is_transparent = void;

        bool
        operator()(
            beast::string_view lhs,
            beast::string_view rhs) const noexcept
        {
            return lhs < rhs;
        }
    };

    server& srv_;
    boost::container::flat_set<
        command, compare> set_;

public:
    explicit
    dispatcher_impl(
        server& srv)
        : srv_(srv)
    {
    }

    void
    insert(
        beast::string_view method,
        handler_type handler) override
    {
        set_.emplace(command{
            method, std::move(handler)});
    }

    void
    dispatch(
        user& u, net::const_buffer b) override
    {
        json::rpc_request req;
        try
        {
            json::parser pr;
            beast::error_code ec;

            // Parse the buffer into JSON
            pr.write(b, ec);
            if(ec)
                throw rpc_exception(
                    json::rpc_error::parse_error,
                    ec.message());

            // Validate and extract the JSON-RPC request
            req.extract(pr.release(), ec);
            if(ec)
                throw rpc_exception(
                    json::rpc_error::invalid_request,
                    ec.message());

            beast::string_view method = req.method;

            // Look up the command
            auto const it = set_.find(method);
            if(it == set_.end())
                throw rpc_exception(
                    json::rpc_error::method_not_found,
                    "Unknown method");

            // Dispatch the RPC command
            it->handler(u, req);
        }
        catch(rpc_exception const& e)
        {
            u.send(make_message(e.to_json(req.id)));
        }
    }
};

} // (anon)

std::unique_ptr<dispatcher>
make_dispatcher(server& srv)
{
    return boost::make_unique<
        dispatcher_impl>(srv);
}
