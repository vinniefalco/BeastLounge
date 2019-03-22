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
#include <boost/make_unique.hpp>

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
        auto& d = srv_.dispatcher();
        d.insert("identify", &ident_service::rpc_set_identity, this);
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
        user& u, json::rpc_request& req)
    {
        auto& name = checked_string(req.params, "name");

        if(name.size() > 20)
            throw rpc_exception(
                "Invalid \"name\": too long");
        if(! u.name.empty())
            throw rpc_exception(
                "Identity is already set");

        // VFALCO NOT THREAD SAFE!
        u.name.assign(name.data(), name.size());

        json::value res;
        res["jsonrpc"] = "2.0";
        res["result"] = 0;
        if(req.id.has_value())
            res["id"] = std::move(*req.id);
        u.send(make_message(res));
    }

};

} // (anon)

//------------------------------------------------------------------------------

void
make_ident_service(server& srv)
{
    srv.insert(boost::make_unique<ident_service>(srv));
}
