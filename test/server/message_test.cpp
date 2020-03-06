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

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <boost/beast/core/buffers_to_string.hpp>

class message_test : public beast::unit_test::suite
{
public:
    void
    testMessage()
    {
        net::const_buffer cb("Hello, world!", 13);
        auto m = message(cb);
        BEAST_EXPECT(
            beast::buffer_bytes(m) == cb.size());
        BEAST_EXPECT(
            beast::buffers_to_string(m) ==
                "Hello, world!");
    }

    void
    run() override
    {
        testMessage();
        pass();
    }
};

BEAST_DEFINE_TESTSUITE(lounge,server,message);
