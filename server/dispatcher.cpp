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
    struct member
    {
        beast::string_view method;
        beast::string_view agent;
    };

    struct command : member
    {
        handler_type handler;

        command(
            beast::string_view method_,
            beast::string_view agent_,
            handler_type&& handler_)
            : member({method_, agent_})
            , handler(std::move(handler_))
        {
        }
    };

    struct compare
    {
        using is_transparent = void;

        bool
        operator()(
            member const& lhs,
            member const& rhs) const noexcept
        {
            if(lhs.method < rhs.method)
                return true;
            else if(lhs.method > rhs.method)
                return false;
            else
                return (lhs.agent < rhs.agent);
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
        beast::string_view agent,
        handler_type handler) override
    {
        set_.emplace(command{
            method, agent, std::move(handler)});
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

            // Extract the agent to route to
            beast::string_view method = req.method;
            beast::string_view agent =
                checked_string(req.params, "agent");

            // Look up the command
            auto const it =
                set_.find(member{method, agent});
            if(it == set_.end())
                throw rpc_exception(
                    json::rpc_error::method_not_found,
                    "Unknown method or agent");

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
