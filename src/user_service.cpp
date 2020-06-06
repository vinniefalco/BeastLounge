//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <lounge/user_service.hpp>
#include <boost/smart_ptr/make_shared.hpp>

namespace lounge {

namespace {

//----------------------------------------------------------

class user_impl : public user
{
    std::unique_ptr<handler> h_;

public:
    explicit
    user_impl(
        std::unique_ptr<handler> h)
        : h_(std::move(h))
    {
    }

    void
    send(message m) override
    {
        h_->do_send(std::move(m));
    }
};

//----------------------------------------------------------

class user_service_impl
    : public user_service
{
    server& srv_;
    log& log_;

public:
    using key_type = user_service;

    user_service_impl(
        server& srv)
        : srv_(srv)
        , log_(srv.get_log("user"))
    {
    }

    ~user_service_impl()
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

    boost::shared_ptr<user>
    create_user_impl(
        std::unique_ptr<handler> h) override
    {
        return boost::make_shared<user_impl>(
            std::move(h));
    }
};

} // (anon)

user_service&
user_service::
install(
    server& srv)
{
    return emplace_service<user_service_impl>(srv);
}

} // lounge
