//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <lounge/channel.hpp>
#include <boost/beast/core/bind_handler.hpp>
#include <boost/asio/post.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <utility>

namespace lounge {

namespace {

//----------------------------------------------------------

class channel_impl
    : public channel
{
    server& srv_;
    executor_type ex_;
    std::unique_ptr<handler> h_;
    boost::container::flat_set<user*> users_;

public:
    channel_impl(
        server& srv,
        std::unique_ptr<handler> h)
        : srv_(srv)
        , ex_(srv.make_executor())
        , h_(std::move(h))
    {
    }

    void
    insert(user& u) override
    {
        auto wp = boost::weak_from(&u);
        net::post(ex_,
            beast::bind_front_handler(
                &channel_impl::do_insert,
                this,
                std::move(wp)));
    }

    void
    erase(user& u) override
    {
        auto wp = boost::weak_from(&u);
        net::post(ex_,
            beast::bind_front_handler(
                &channel_impl::do_erase,
                this,
                std::move(wp)));
    }

    void
    do_insert(
        boost::weak_ptr<user> wp)
    {
        auto sp = wp.lock();
        users_.insert(sp.get());
        h_->on_insert(*sp);
    }

    void
    do_erase(
        boost::weak_ptr<user> wp)
    {
        auto sp = wp.lock();
        users_.erase(sp.get());
        h_->on_erase(*sp);
    }

    //------------------------------------------------------
};

} // (anon)

boost::shared_ptr<channel>
channel::
create_impl(
    server& srv,
    std::unique_ptr<handler> h)
{
    return boost::make_shared<
        channel_impl>(srv, std::move(h));
}

} // lounge
