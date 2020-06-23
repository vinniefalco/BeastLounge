//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_IMPL_USER_SERVICE_HPP
#define LOUNGE_IMPL_USER_SERVICE_HPP

#include <utility>

namespace lounge {

struct user::handler
{
    virtual ~handler() = default;
    virtual void do_send(message m) = 0;
};

template<class Handler>
boost::shared_ptr<user>
user_service::
create_user(
    boost::weak_ptr<void> c,
    Handler&& h)
{
    struct handler_impl : user::handler
    {
        Handler h;

        explicit
        handler_impl(
            Handler&& h_)
            : h(std::forward<
                Handler>(h_))
        {
        }

        void
        do_send(
            message m) override
        {
            h.do_send(std::move(m));
        }
    };

    return create_user_impl(
        std::move(c),
        std::unique_ptr<handler_impl>(
            new handler_impl(
                std::forward<Handler>(h))));
}

} // lounge

#endif
