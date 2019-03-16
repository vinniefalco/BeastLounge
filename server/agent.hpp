//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_AGENT_HPP
#define LOUNGE_AGENT_HPP

#include "config.hpp"
#include "session.hpp"
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <mutex>
#include <vector>

/** Base for polymorphic services

    The lifetime of each agent is the
    same as the server lifetime.
*/
class agent
{
protected:
    std::mutex mutex_;
    boost::container::flat_set<
        session*> sessions_;

    std::vector<
        boost::weak_ptr<session>>
    release_sessions();

public:
    virtual ~agent();

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

    /** Called when statistics are gathered
    */
    virtual
    void
    on_stat(json::value& jv) = 0;

    /** Add a session to the agent
    */
    void
    insert(session* p);

    /** Remove a session from the agent
    */
    void
    erase(session* p);
};

#endif
