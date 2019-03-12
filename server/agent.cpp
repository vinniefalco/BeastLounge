//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//
 
#include "agent.hpp"

auto
agent::
release_sessions() ->
    std::vector<
        boost::weak_ptr<session>>
{
    std::vector<
        boost::weak_ptr<session>> v;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        v.reserve(sessions_.size());
        for(auto p : sessions_)
            v.emplace_back(p->get_weak_ptr());
        sessions_.clear();
        sessions_.shrink_to_fit();
    }
    return v;
}

//---

agent::
~agent()
{
    BOOST_ASSERT(sessions_.empty());
}

void
agent::
insert(session* p)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.insert(p);
}

void
agent::
remove(session* p)
{
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(p);
}
