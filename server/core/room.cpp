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
    on_dispatch(rpc_call& rpc) override
    {
        if(rpc.method == "say")
        {
            do_say(rpc);
        }
        else if(rpc.method == "slash")
        {
            do_say(rpc);
        }
        else
        {
            rpc.fail(rpc_code::method_not_found);
        }
    }

    //--------------------------------------------------------------------------
    //
    // room_impl
    //
    //--------------------------------------------------------------------------

    void
    do_say(rpc_call& rpc)
    {
        checked_user(rpc);
        if(! is_joined(*rpc.u))
            rpc.fail("not in channel");
        auto const& text =
            checked_string(rpc.params, "message");
        {
            // broadcast: say
            json::value jv(json::object_kind);
            auto& obj = jv.get_object();
            obj["verb"] = "say";
            obj["cid"] = cid();
            obj["name"] = name();
            obj["user"] = rpc.u->name;
            obj["message"] = text;
            send(jv);
        }
        rpc.complete();
    }

    void
    do_slash(rpc_call& rpc)
    {
        checked_user(rpc);
        rpc.fail("Unimplemented");
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
