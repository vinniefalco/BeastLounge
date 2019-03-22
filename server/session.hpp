//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SESSION_HPP
#define LOUNGE_SESSION_HPP

#include "config.hpp"
#include "utility.hpp"

/** Base for polymorphic connections

    Every session must be owned by one listener
*/
class session
    : public enable_shared_from
{
public:
    virtual ~session() = default;

    /** Called when the server stops.

        This will be called at most once.
    */
    virtual
    void
    on_stop() = 0;
};

#endif
