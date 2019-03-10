//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/key_param.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>

namespace boost {
namespace beast {
namespace json {

class key_param_test : public unit_test::suite
{
public:
    void
    testMembers()
    {
        {
            key_param("x");
        }
        pass();
    }

    void run() override
    {
        testMembers();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,key_param);

} // json
} // beast
} // boost
