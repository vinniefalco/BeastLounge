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
#include <boost/smart_ptr/shared_ptr.hpp>

/** Base for polymorphic connections

    Every session must be owned by one service
*/
class session
{
public:
    virtual ~session() = default;

    /// Returns a weak pointer to the session
    virtual
    boost::weak_ptr<session>
    get_weak_session_ptr() = 0;

    /** Called when the server stops.

        This will be called at most once.
    */
    virtual
    void
    on_stop() = 0;
};

#endif
