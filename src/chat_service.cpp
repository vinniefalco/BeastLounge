//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <lounge/chat_service.hpp>
#include <lounge/channel.hpp>

namespace lounge {

namespace {

struct channel_handler
{
    void
    on_insert(
        channel&,
        user&)
    {
    }

    void
    on_erase(
        channel&,
        user&)
    {
    }
};

//----------------------------------------------------------

class chat_service_impl
    : public chat_service
{
    server& srv_;
    log& log_;

public:
    using key_type = chat_service;

    chat_service_impl(
        server& srv)
        : srv_(srv)
        , log_(srv.get_log("chat"))
    {
    }

    ~chat_service_impl()
    {
    }

    void
    on_start() override
    {
    }

    void
    on_stop() override
    {
    }

    //------------------------------------------------------
};

} // (anon)

chat_service&
chat_service::
install(
    server& srv)
{
    return emplace_service<chat_service_impl>(srv);
}

} // lounge
