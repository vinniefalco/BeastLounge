//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "server.hpp"
#include "service.hpp"
#include <boost/container/flat_set.hpp>
#include <boost/shared_ptr.hpp>
#include <mutex>
#include <vector>

//------------------------------------------------------------------------------

namespace {

class chat_service
    : public service
{
    server& srv_;

public:
    chat_service(
        server& srv)
        : srv_(srv)
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

    void
    on_stat(json::value& jv) override
    {
        boost::ignore_unused(jv);
    }
};

} // (anon)

//------------------------------------------------------------------------------

bool
make_chat_service(
    server& srv)
{
    auto sp = boost::make_shared<
        chat_service>(srv);
    srv.insert(std::move(sp));
    return true;
}
