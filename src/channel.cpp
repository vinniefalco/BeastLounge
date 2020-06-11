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
    boost::container::flat_set<
        boost::shared_ptr<user>> users_;

public:
    channel_impl(
        server& srv,
        std::unique_ptr<handler> h)
        : srv_(srv)
        , ex_(srv.make_executor())
        , h_(std::move(h))
    {
    }

    std::size_t
    size() override
    {
        return users_.size();
    }

    user&
    at(std::size_t i) override
    {
        return **users_.nth(i);
    }

    void
    insert(user& u) override
    {
        net::post(ex_,
            beast::bind_front_handler(
                &channel_impl::do_insert,
                this,
                boost::shared_from(&u)));
    }

    void
    erase(user& u) override
    {
        net::post(ex_,
            beast::bind_front_handler(
                &channel_impl::do_erase,
                this,
                boost::shared_from(&u)));
    }

    void
    do_insert(
        boost::shared_ptr<user> sp)
    {
        users_.emplace(std::move(sp));
        h_->on_insert(*this, *sp);
    }

    void
    do_erase(
        boost::shared_ptr<user> sp)
    {
        users_.erase(sp);
        h_->on_erase(*this, *sp);
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
