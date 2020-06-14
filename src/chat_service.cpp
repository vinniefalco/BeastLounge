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

    // temporary hard-coded channel for now
    boost::shared_ptr<channel> ch_;

public:
    using key_type = chat_service;

    chat_service_impl(
        server& srv)
        : srv_(srv)
        , log_(srv.get_log("chat"))
        , ch_(channel::create(
            srv, channel_handler{}))
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

    void
    insert(user& u)
    {
        ch_->insert(u);
    }

    void
    on_msg(string_view s)
    {
        ch_->send(net::const_buffer(
            s.data(), s.size()));
    }
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
