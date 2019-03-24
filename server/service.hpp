//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SERVICE_HPP
#define LOUNGE_SERVICE_HPP

#include "config.hpp"
#include <boost/beast/_experimental/json/value.hpp>

/** Base for polymorphic services

    The lifetime of each service is the
    same as the server lifetime.
*/
class service
{
public:
    virtual ~service() = default;

    /** Called when the server starts.

        This will be called at most once.
    */
    virtual
    void
    on_start() = 0;

    /** Called when the server stops.

        This will be called at most once.
    */
    virtual
    void
    on_stop() = 0;
};

#endif
