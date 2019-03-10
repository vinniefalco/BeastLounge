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
#include "shared_state.hpp"
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <cstdlib>
#include <memory>

/** Represents an established HTTP connection
*/
class http_session
    : public std::enable_shared_from_this<http_session>
{
    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    std::shared_ptr<shared_state> state_;
    http::request<http::string_body> req_;

    void
    fail(
        beast::error_code ec,
        char const* what);

    void
    on_read(
        beast::error_code ec,
        std::size_t);

    void
    on_write(
        beast::error_code ec,
        std::size_t,
        bool close);

public:
    http_session(
        tcp::socket sock,
        std::shared_ptr<shared_state> const& state);

    void run();
};

#endif
