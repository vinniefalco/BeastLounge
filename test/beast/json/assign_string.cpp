//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/assign_string.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class assign_string_test : public unit_test::suite
{
public:
    void
    testAssign()
    {
        value jv = "test";
        std::string s;
        error_code ec;
        jv.assign(s, ec);
        BEAST_EXPECTS(! ec, ec.message());
        BEAST_EXPECT(s == "test");
    }

    void run() override
    {
        testAssign();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,assign_string);

} // json
} // beast
} // boost
