//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "any_connection.hpp"
#include <boost/assert.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/make_unique.hpp>
#include <atomic>
#include <mutex>

namespace lounge {

namespace {

class list_impl
    : public any_connection::list
{
    std::mutex m_;
    std::atomic<bool> open_ = true;
    boost::container::flat_set<
        any_connection*> v_;

public:
    ~list_impl()
    {
        std::lock_guard<
            std::mutex> lock(m_);
        BOOST_ASSERT(v_.empty());
    }

    bool
    is_open() const noexcept override
    {
        return open_;
    }

    void
    close() override
    {
        open_ = false;

        // Transfer ownership of the
        // container to the stack.
        decltype(v_) v;
        {
            std::lock_guard<
                std::mutex> lock(m_);
            v = std::move(v_);
        }
        for(auto& c : v)
            c->close();
    }

    bool
    insert(any_connection& c) override
    {
        std::lock_guard<
            std::mutex> lock(m_);
        if(! open_)
            return false;
        v_.insert(&c);
        set_list(c);
        return true;
    }

    virtual
    void
    erase(any_connection& c) override
    {
        std::lock_guard<
            std::mutex> lock(m_);
        v_.erase(&c);
    }
};

} // (anon)

any_connection::
~any_connection()
{
    BOOST_ASSERT(list_);
    list_->erase(*this);
}

std::unique_ptr<
    any_connection::list>
any_connection::
list::
create()
{
    return boost::make_unique<list_impl>();
}

} // lounge
