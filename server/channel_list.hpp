//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CHANNEL_LIST_HPP
#define LOUNGE_CHANNEL_LIST_HPP

#include "config.hpp"
#include "uid.hpp"
#include <cstdlib>
#include <boost/asio/buffer.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <utility>

class channel;
class rpc_request;
class rpc_response;
class user;

//------------------------------------------------------------------------------

class channel_list
{
public:
    virtual
    ~channel_list() = default;

    virtual
    uid_t
    next_uid() noexcept = 0;

    virtual
    std::size_t
    next_cid() noexcept = 0;

    /// Return the channel for a cid, or nullptr
    virtual
    boost::shared_ptr<channel>
    at(std::size_t cid) const = 0;

    /// Process a serialized message from a user
    virtual
    void
    dispatch(
        net::const_buffer b,
        user& u) = 0;

    template<class T, class...  Args>
    friend
    void
    insert(
        channel_list& list,
        Args&&... args);

    virtual
    void
    erase(channel const& c) = 0;

private:
    virtual
    void
    insert(boost::shared_ptr<channel> c) = 0;
};

template<class T, class...  Args>
void
insert(
    channel_list& list,
    Args&&... args)
{
    list.insert(boost::make_shared<T>(
        std::forward<Args>(args)...));
}

#endif
