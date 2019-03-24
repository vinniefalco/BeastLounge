//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/core/expected.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <string>
#include <type_traits>

namespace boost {
namespace beast {

class expected_test : public unit_test::suite
{
public:
    using str = std::string;

    struct throw_on_copy
    {
        throw_on_copy() = default;
        throw_on_copy& operator=(
            throw_on_copy const&) = delete;

        throw_on_copy(
            throw_on_copy&&)
        {
        }

        throw_on_copy(
            throw_on_copy const&)
        {
            throw std::exception();
        }
    };

    void
    testSpecial()
    {
        // move construct
        {
            expected<str> e1("x");
            expected<str> e2(std::move(e1));
            BEAST_EXPECT(e1->size() == 0);
            BEAST_EXPECT(e2->size() == 1);
            BEAST_EXPECT(*e2 == "x");
        }

        // copy construct
        {
            expected<str> e1("x");
            expected<str> e2(e1);
            BEAST_EXPECT(e1->size() == 1);
            BEAST_EXPECT(e2->size() == 1);
            BEAST_EXPECT(*e1 == "x");
            BEAST_EXPECT(*e2 == "x");
        }

        // move assign
        {
            expected<str> e1("x");
            expected<str> e2("");
            e2 = std::move(e1);
            BEAST_EXPECT(e1->size() == 0);
            BEAST_EXPECT(e2->size() == 1);
            BEAST_EXPECT(*e2 == "x");
        }

        // copy assign
        {
            expected<str> e1("x");
            expected<str> e2("");
            e2 = e1;
            BEAST_EXPECT(e1->size() == 1);
            BEAST_EXPECT(e2->size() == 1);
            BEAST_EXPECT(*e1 == "x");
            BEAST_EXPECT(*e2 == "x");
        }

        // throw on copy assign
        {
            expected<throw_on_copy> e1{
                throw_on_copy{}};
            expected<throw_on_copy> e2{
                throw_on_copy{}};
            BEAST_EXPECT(e2);
            try
            {
                e2 = e1;
                BEAST_FAIL();
            }
            catch(std::exception const&)
            {
                BEAST_PASS();
            }
            catch(...)
            {
                BEAST_PASS();
            }
            BEAST_EXPECT(! e2);
        }
    }

    void
    testMembers()
    {
        // move construct from value
        {
            expected<str> e(str("x"));
            BEAST_EXPECT(e.value() == "x");
        }

        // copy construct from value
        {
            str s = "x";
            expected<str> e(s);
            BEAST_EXPECT(e.value() == "x");
        }

        // construct from implicit value
        {
            expected<str> e("x");
            BEAST_EXPECT(e.value() == "x");
        }

        // in-place construct value
        {
            expected<str> e(
                boost::in_place_init, "x");
            BEAST_EXPECT(e.value() == "x");
        }

        // construct from error_code
        {
            expected<str> e(
                error_code{error::timeout});
            BEAST_EXPECT(
                e.error() == error::timeout);
        }

        // construct from implicit error_code
        {
            expected<str> e(error::timeout);
            BEAST_EXPECT(
                e.error() == error::timeout);
        }

        // assign from value
        {
            expected<str> e = error_code();
            BEAST_EXPECT(! e.has_value());
            e = str("x");
            BEAST_EXPECT(e.value() == "x");
        }

        // assign from implicit value
        {
            expected<str> e = error_code();
            e = "x";
            BEAST_EXPECT(e.value() == "x");
        }

        // assign from implicit error
        {
            expected<str> e = error_code();
            e = error::timeout;
            BEAST_EXPECT(
                e.error() == error::timeout);
        }
    }

    void
    testObservers()
    {
        {
            expected<str> e("x");
            auto const& e1(e);
            expected<str> e2(error::timeout);
            BEAST_EXPECT(e->size() == 1);
            BEAST_EXPECT(e1->size() == 1);
            BEAST_EXPECT(*e == "x");
            BEAST_EXPECT(*e1 == "x");
            BEAST_EXPECT(e);
            BEAST_EXPECT(e1);
            BEAST_EXPECT(! e2);
            BEAST_EXPECT(e.value() == "x");
            BEAST_EXPECT(e1.value() == "x");
            BEAST_EXPECT(
                e2.error() == error::timeout);
            BEAST_EXPECT(
                e1.value_or("y") == "x");
            BEAST_EXPECT(std::move(
                e).value_or("y") == "x");
            BEAST_EXPECT(e->empty());
            BEAST_EXPECT(
                e2.value_or("y") == "y");
        }
    }

    void
    testEquality()
    {
        expected<str> s1("x");
        expected<str> s2("x");
        expected<str> s3("y");
        expected<str> s4(error::timeout);
        BEAST_EXPECT(s1 == s2);
        BEAST_EXPECT(s1 != s3);
        BEAST_EXPECT(s1 != s4);
        BEAST_EXPECT(s1 != error::timeout);
        BEAST_EXPECT(error::timeout != s1);
        BEAST_EXPECT(s4 == error::timeout);
        BEAST_EXPECT(error::timeout == s4);
    }

    // VFALCO TODO
#if 0
    expected<str>
    testReturn()
    {
        return error::timeout;
    }
#endif

    void
    run() override
    {
        testSpecial();
        testMembers();
        testObservers();
        testEquality();
        //testReturn();
    }
};

BEAST_DEFINE_TESTSUITE(beast,core,expected);

} // beast
} // boost
