//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <lounge/channel.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <utility>

namespace lounge {

namespace {

//----------------------------------------------------------

class channel_impl
    : public channel
{
    std::unique_ptr<handler> h_;

public:
    explicit
    channel_impl(
        std::unique_ptr<handler> h)
        : h_(std::move(h))
    {
    }

    //------------------------------------------------------
};

} // (anon)

boost::shared_ptr<channel>
channel::
create_impl(std::unique_ptr<handler> h)
{
    return boost::make_shared<
        channel_impl>(std::move(h));
}

} // lounge
