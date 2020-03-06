//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include "core/message.hpp"

#include <boost/beast/core/buffers_to_string.hpp>

#include "test_suite.hpp"

class message_test
{
public:
    void
    testMessage()
    {
        net::const_buffer cb("Hello, world!", 13);
        auto m = message(cb);
        BOOST_TEST(
            beast::buffer_bytes(m) == cb.size());
        BOOST_TEST(
            beast::buffers_to_string(m) ==
                "Hello, world!");
    }

    void
    run()
    {
        testMessage();
    }
};

TEST_SUITE(message_test, "lounge.server.message");
