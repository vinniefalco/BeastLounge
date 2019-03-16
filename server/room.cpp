//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "room.hpp"
#include <boost/container/flat_set.hpp>
#include <mutex>
#include <vector>

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

namespace {

class room_impl : public room
{
    std::mutex mutex_;
    boost::container::flat_set<
        ws_session*> sessions_;

public:
    explicit
    room_impl()
    {
    }

    void
    insert(ws_session* ws) override
    {
        std::lock_guard<
            std::mutex> lock(mutex_);
        sessions_.insert(ws);
    }

    void
    erase(ws_session* ws) override
    {
        std::lock_guard<
            std::mutex> lock(mutex_);
        sessions_.erase(ws);
    }

    void
    send(message m) override
    {
        // Make a local list of all the weak pointers
        // representing the sessions, so we can do the
        // actual sending without holding the mutex:
        std::vector<boost::weak_ptr<ws_session>> v;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            v.reserve(sessions_.size());
            for(auto p : sessions_)
                v.emplace_back(p->get_weak_ptr());
        }

        // For each session in our local list, try to
        // acquire a strong pointer. If successful,
        // then send the message on that session.
        for(auto const& wp : v)
            if(auto sp = wp.lock())
                sp->send(m);
    };
};

} // (anon)

//------------------------------------------------------------------------------

boost::shared_ptr<room>
make_room()
{
    return boost::make_shared<
        room_impl>();
}
