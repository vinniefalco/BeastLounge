//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_CHANNEL_SERVICE_HPP
#define SRC_CHANNEL_SERVICE_HPP

#include <lounge/server.hpp>
#include <lounge/user.hpp>

namespace lounge {

class channel_service
    : public service
{
public:
    static
    void
    install(
        server& srv);
};

} // lounge

#endif
