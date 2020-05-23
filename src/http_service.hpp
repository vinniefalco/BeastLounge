//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_HTTP_SERVICE_HPP
#define SRC_HTTP_SERVICE_HPP

#include "http_generator.hpp"
#include <lounge/server.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>

namespace lounge {

class http_service
    : public service
{
public:
    virtual
    http_generator
    handle_request(
        http::request<http::string_body>&& req) = 0;

    static
    void
    install(
        server& srv);
};

} // lounge

#endif
