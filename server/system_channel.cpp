//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "rpc.hpp"
#include "channel.hpp"
#include "channel_list.hpp"
#include "server.hpp"
#include "system_channel.hpp"
#include "user.hpp"
#include <boost/make_shared.hpp>

//------------------------------------------------------------------------------

namespace {

class system_channel : public channel
{
    server& srv_;

public:
    explicit
    system_channel(server& srv)
        :  channel(
            1,
            "System",
            srv.channel_list())
        , srv_(srv)
    {
    }

protected:
    void
    on_insert(user&) override
    {
    }

    void
    on_erase(user&) override
    {
    }

    void
    on_dispatch(
        json::value& result,
        rpc_request& req,
        user& u) override
    {
        if(req.method == "identify")
        {
            do_identify(result, req, u);
        }
        else if(req.method == "shutdown")
        {
            do_shutdown(result, req, u);
        }
        else if(req.method == "stop")
        {
            do_stop(result, req, u);
        }
        else
        {
            throw rpc_except{rpc_error::method_not_found};
        }
    }

    void
    do_identify(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result);

        auto& name = checked_string(req.params, "name");

        if(name.size() > 20)
            throw rpc_except(
                "Invalid \"name\": too long");
        if(! u.name.empty())
            throw rpc_except(
                "Identity is already set");

        // VFALCO NOT THREAD SAFE!
        u.name.assign(name.data(), name.size());

        insert(u);
    }

    void
    do_shutdown(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req, u);
        srv_.shutdown(
            std::chrono::seconds(30));
    }

    void
    do_stop(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req, u);
        srv_.stop();
    }
};

} // (anon)

void
make_system_channel(server& srv)
{
    insert<system_channel>(
        srv.channel_list(),
        srv);
}
