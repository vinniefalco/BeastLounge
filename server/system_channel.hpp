//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SYSTEM_CHANNEL_HPP
#define LOUNGE_SYSTEM_CHANNEL_HPP

#include "config.hpp"
#include "channel.hpp"

class system_channel : public channel
{
protected:
    void
    on_insert(user& u) override;

    void
    on_erase(user& u) override;
};

#endif