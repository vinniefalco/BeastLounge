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
#include "service.hpp"
#include "user.hpp"
#include <boost/shared_ptr.hpp>

//------------------------------------------------------------------------------

namespace {

class ident_service
    : public service
{
    server& srv_;

public:
    explicit
    ident_service(
        server& srv)
        : srv_(srv)
    {
    }

    //--------------------------------------------------------------------------
    //
    // service
    //
    //--------------------------------------------------------------------------

    void
    on_start() override
    {
        // Register RPC commands
        srv_.dispatcher().insert(
            "set-identity", "ident",
            [&](user& u, json::rpc_request&& req)
            {
                this->rpc_set_identity(u, std::move(req));
            });
    }

    void
    on_stop() override
    {
    }

    void
    on_stat(json::value& jv) override
    {
        boost::ignore_unused(jv);
    }

    //--------------------------------------------------------------------------

    void
    rpc_set_identity(
        user& u, json::rpc_request&& req)
    {
        auto& params = req.params.as_object();
        auto it = params.find("name");
        if(it == params.end())
        {
            u.send(make_message(make_rpc_error(
                json::rpc_error::invalid_params,
                "Missing parameter \"agent\"",
                req)));
            return;
        }
        if(! it->second.is_string())
        {
            u.send(make_message(make_rpc_error(
                json::rpc_error::invalid_params,
                "Expected string for parameter \"agent\"",
                req)));
            return;
        }
        auto& str = it->second.as_string();
        if(str.empty())
        {
            u.send(make_message(make_rpc_error(
                json::rpc_error::invalid_params,
                "Invalid \"name\" = \"" + str + "\"",
                req)));
            return;
        }
        if(str.size() > 20)
        {
            u.send(make_message(make_rpc_error(
                json::rpc_error::invalid_params,
                "Invalid \"name\": too long",
                req)));
            return;
        }
        if(! u.name.empty())
        {
            u.send(make_message(make_rpc_error(
                json::rpc_error::invalid_params,
                "Identity already set",
                req)));
            return;
        }

        // VFALCO NOT THREAD SAFE!
        u.name.assign(str.data(), str.size());
    }

};

} // (anon)

//------------------------------------------------------------------------------

void
make_ident_service(server& srv)
{
    auto sp = boost::make_shared<
        ident_service>(srv);
    srv.insert(std::move(sp));
}
