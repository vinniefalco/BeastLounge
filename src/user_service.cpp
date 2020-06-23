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

#include <lounge/channel.hpp>
#include <boost/container/flat_set.hpp>

namespace lounge {

namespace {

//----------------------------------------------------------

class user_impl : public user
{
    boost::weak_ptr<void> c_;
    std::unique_ptr<handler> h_;
    boost::container::flat_set<
        boost::shared_ptr<channel>> channels_;

public:
    explicit
    user_impl(
        boost::weak_ptr<void> c,
        std::unique_ptr<handler> h)
        : c_(std::move(c))
        , h_(std::move(h))
    {
    }

    void
    send(message m) override
    {
        h_->do_send(std::move(m));
    }

    void
    on_disconnect() override
    {
        //for(auto const& sp :
    }
};

//----------------------------------------------------------

class user_service_impl
    : public user_service
{
    server& srv_;
    log& log_;
    int data_index_ = 0;

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
        boost::weak_ptr<void> c,
        std::unique_ptr<handler> h) override
    {
        return boost::make_shared<user_impl>(
            std::move(c), std::move(h));
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
