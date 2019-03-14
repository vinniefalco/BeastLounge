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

namespace {

enum class te
{
    a, b, c
};

string_view
make_key_string(te e)
{
    switch(e)
    {
    case te::a: return "a";
    case te::b: return "b";
    case te::c:
        break;
    }
    return "c";
}

} // (anon)

template<>
struct is_key_enum<te> : std::true_type
{
};

class key_param_test : public unit_test::suite
{
public:
    using kp = key_param;

    BOOST_STATIC_ASSERT(
        is_key_enum<te>::value);

    static
    string_view
    ks(string_view s)
    {
        return s;
    }

    void
    testMembers()
    {
        BEAST_EXPECT(kp("x") == "x");
        BEAST_EXPECT(kp("y") == "y");
        BEAST_EXPECT(kp(te::a) == "a");
        BEAST_EXPECT(kp(te::b) == "b");
        BEAST_EXPECT(kp(te::c) == "c");

        BEAST_EXPECT(! (kp("a") == kp("b")));
        BEAST_EXPECT(   kp("a") != kp("b"));
        BEAST_EXPECT(   kp("a") <  kp("b"));
        BEAST_EXPECT(   kp("a") <= kp("b"));
        BEAST_EXPECT(! (kp("a") >  kp("b")));
        BEAST_EXPECT(! (kp("a") >= kp("b")));

        BEAST_EXPECT(! (kp(te::a) == kp("b")));
        BEAST_EXPECT(   kp(te::a) != kp("b"));
        BEAST_EXPECT(   kp(te::a) <  kp("b"));
        BEAST_EXPECT(   kp(te::a) <= kp("b"));
        BEAST_EXPECT(! (kp(te::a) >  kp("b")));
        BEAST_EXPECT(! (kp(te::a) >= kp("b")));

        BEAST_EXPECT(! (kp("a") == kp(te::b)));
        BEAST_EXPECT(   kp("a") != kp(te::b));
        BEAST_EXPECT(   kp("a") <  kp(te::b));
        BEAST_EXPECT(   kp("a") <= kp(te::b));
        BEAST_EXPECT(! (kp("a") >  kp(te::b)));
        BEAST_EXPECT(! (kp("a") >= kp(te::b)));

        BEAST_EXPECT(! (kp(te::a) == kp(te::b)));
        BEAST_EXPECT(   kp(te::a) != kp(te::b));
        BEAST_EXPECT(   kp(te::a) <  kp(te::b));
        BEAST_EXPECT(   kp(te::a) <= kp(te::b));
        BEAST_EXPECT(! (kp(te::a) >  kp(te::b)));
        BEAST_EXPECT(! (kp(te::a) >= kp(te::b)));

        BEAST_EXPECT(ks(kp("a")) == "a");
        BEAST_EXPECT(ks(kp(te::b)) == "b");
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
