//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel_service.hpp"

namespace lounge {

//----------------------------------------------------------

class channel_service_impl
    : public channel_service
    , public listener::handler
{
    server& srv_;
    log& log_;

public:
    using key_type = channel_service;

    channel_service_impl(
        server& srv)
        : srv_(srv)
        , log_(srv.get_log("channel"))
        , list_(any_connection::list::create())
    {
        install_rpcs();
    }

    ~connection_service_impl()
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

void
channel_service::
install(
    server& srv)
{
    emplace_service<channel_service_impl>(srv);
}

} // lounge
