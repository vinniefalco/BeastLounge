//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CHANNEL_HPP
#define LOUNGE_CHANNEL_HPP

#include <lounge/server.hpp>
#include <lounge/user.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <memory>

namespace lounge {

class channel
    : public boost::enable_shared_from
{
public:
    virtual ~channel() = default;

    LOUNGE_DECL
    virtual
    void
    insert(user& u) = 0;

    LOUNGE_DECL
    virtual
    void
    erase(user& u) = 0;

    template<class Handler>
    static
    boost::shared_ptr<channel>
    create(
        server& srv,
        Handler&& handler);

protected:
    struct handler;

    LOUNGE_DECL
    static
    boost::shared_ptr<channel>
    create_impl(
        server& srv,
        std::unique_ptr<handler>);
};

} // lounge

#include <lounge/impl/channel.hpp>

#endif
