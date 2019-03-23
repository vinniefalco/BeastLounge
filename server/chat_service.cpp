//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "dispatcher.hpp"
#include "rpc.hpp"
#include "server.hpp"
#include "service.hpp"
#include "user.hpp"
#include <boost/container/flat_set.hpp>
#include <boost/make_unique.hpp>
#include <mutex>
#include <vector>

//------------------------------------------------------------------------------

namespace {

class room : public channel
{
    server& srv_;
    
public:
    explicit
    room(server& srv)
        : srv_(srv)
    {
    }

    void
    on_insert(user& u) override
    {
        // indicates a user joined the room
        json::value jv;
        jv["channel"] = cid();
        jv["verb"] = "join";
        jv["user"] = u.name;
        send(jv);
    }

    void
    on_erase(user& u) override
    {
        // indicates a user left the room
        json::value jv;
        jv["channel"] = cid();
        jv["verb"] = "leave";
        jv["user"] = u.name;
        send(jv);
    }
};

//------------------------------------------------------------------------------

class chat_service
    : public service
{
    server& srv_;
    room room_;

public:
    chat_service(
        server& srv)
        : srv_(srv)
        , room_(srv)
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
        auto& d = srv_.dispatcher();
        // Register RPC commands
        d.insert("join",  &chat_service::rpc_join, this);
        d.insert("say",   &chat_service::rpc_say, this);
        d.insert("slash", &chat_service::rpc_slash, this);
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
    checked_user(user& u)
    {
        if(u.name.empty())
            throw rpc_exception(
                "No identity set");
    }

    // join a room
    void
    rpc_join(user& u, rpc_request& req)
    {
        checked_user(u);

        if(! room_.insert(u))
            throw rpc_exception(
                "Already joined");

        if(req.id.has_value())
        {
            json::value res;
            res["jsonrpc"] = "2.0";
            res["result"] = 0;
                res["id"] = *req.id;
            u.send(res);
        }
    }

    // say something in a room
    void
    rpc_say(
        user& u, rpc_request& req)
    {
        checked_user(u);
        auto const& text =
            checked_string(req.params, "message");

        {
            json::value jv;
            jv["verb"] = "say";
            jv["channel"] = room_.cid();
            jv["user"] = u.name;
            jv["message"] = text;
            room_.send(jv);
        }

        if(req.id.has_value())
        {
            json::value res;
            res["jsonrpc"] = "2.0";
            res["result"] = 0;
                res["id"] = *req.id;
            u.send(res);
        }
    }

    // perform slash command
    void
    rpc_slash(
        user& u, rpc_request&)
    {
        checked_user(u);
        throw rpc_exception(
            "Unimplemented");
    }
};

} // (anon)

//------------------------------------------------------------------------------

void
make_chat_service(
    server& srv)
{
    srv.insert(boost::make_unique<chat_service>(srv));
}
