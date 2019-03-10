//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/exchange.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>

namespace boost {
namespace beast {
namespace json {

class exchange_test : public unit_test::suite
{
public:
#if 0
    BOOST_STATIC_ASSERT(is_exchange_type<object_t>::value);
    BOOST_STATIC_ASSERT(is_exchange_type<array_t>::value);
    BOOST_STATIC_ASSERT(is_exchange_type<null_t>::value);
#endif

    void
    run()
    {
        pass();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,exchange);

} // json
} // beast
} // boost
