//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_HTTP_SESSION_HPP
#define LOUNGE_HTTP_SESSION_HPP

#include "config.hpp"
#include "server.hpp"
#include <memory>

/** Represents an active HTTP connection
*/
class http_session
{
public:
    virtual
    ~http_session() = default;

    virtual
    void
    run() = 0;
};

extern
boost::shared_ptr<http_session>
make_http_session(
    server& srv,
    agent& ag,
    stream_type stream,
    stream_type::endpoint_type ep,
    flat_storage storage);

#endif
