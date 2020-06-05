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
#include <boost/smart_ptr/enable_shared_from.hpp>

namespace lounge {

class channel
    : public boost::enable_shared_from
{
public:
    virtual ~channel() = default;

    template<class Handler>

};

} // lounge

#endif
