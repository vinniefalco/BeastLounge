//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "channel.hpp"
#include "user.hpp"
#include <boost/container/flat_set.hpp>
#include <boost/make_unique.hpp>
#include <boost/weak_ptr.hpp>
#include <mutex>
#include <vector>

//------------------------------------------------------------------------------

namespace {

class channel_impl : public channel
{
    std::mutex mutex_;
    boost::container::flat_set<user*> users_;

public:
    explicit
    channel_impl()
    {
    }

    void
    insert(user* u) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        users_.insert(u);
    }

    void
    erase(user* u) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        users_.erase(u);
    }

    void
    send(message m) override
    {
        // Make a local list of all the weak pointers
        // representing the sessions, so we can do the
        // actual sending without holding the mutex:
        std::vector<boost::weak_ptr<user>> v;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            v.reserve(users_.size());
            for(auto p : users_)
                v.emplace_back(p->get_weak_ptr());
        }

        // For each user in our local list, try to
        // acquire a strong pointer. If successful,
        // then send the message to that user.
        for(auto const& wp : v)
            if(auto sp = wp.lock())
                sp->send(m);
    };
};

} // (anon)

//------------------------------------------------------------------------------

std::unique_ptr<channel>
make_channel()
{
    return boost::make_unique<
        channel_impl>();
}
