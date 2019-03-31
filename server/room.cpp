//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "channel_list.hpp"
#include "rpc.hpp"
#include "user.hpp"

namespace {

class room_impl : public channel
{
public:
    room_impl(
        beast::string_view name,
        channel_list& list)
        : channel(
            2,
            name,
            list)
    {
    }

    //--------------------------------------------------------------------------
    //
    // channel
    //
    //--------------------------------------------------------------------------

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
        if(req.method == "say")
        {
            do_say(result, req, u);
        }
        else if(req.method == "slash")
        {
            do_say(result, req, u);
        }
        else
        {
            throw rpc_error{rpc_code::method_not_found};
        }
    }

    //--------------------------------------------------------------------------
    //
    // room_impl
    //
    //--------------------------------------------------------------------------

    void
    do_say(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result);

        checked_user(u);
        auto const& text =
            checked_string(req.params, "message");

        if(! is_joined(u))
            throw rpc_error{"not in channel"};

        {
            // broadcast: say
            json::value jv;
            jv["verb"] = "say";
            jv["cid"] = cid();
            jv["name"] = name();
            jv["user"] = u.name;
            jv["message"] = text;
            send(jv);
        }
    }

    void
    do_slash(
        json::value& result,
        rpc_request& req,
        user& u)
    {
        boost::ignore_unused(result, req);
        checked_user(u);
        throw rpc_error(
            "Unimplemented");
    }
};

} // (anon)

void
make_room(
    channel_list& list,
    beast::string_view name)
{
    insert<room_impl>(list, name, list);
}
