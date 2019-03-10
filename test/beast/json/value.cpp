//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/value.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <memory>

namespace boost {
namespace beast {
namespace json {

class value_test : public unit_test::suite
{
public:
    BOOST_STATIC_ASSERT(
        is_exchange_type<int>::value);
    void
    testMembers()
    {
        // default ctor
        {
            value jv;
        }

        // move ctor
        {
            value jv1;
            value jv2(std::move(jv1));
        }

        // copy ctor
        {
            value jv1;
            value jv2(jv1);
        }

        // move assign
        {
            value jv1;
            value jv2;
            jv2 = std::move(jv1);
        }

        // copy assign
        {
            value jv1;
            value jv2;
            jv2 = jv1;
        }

        // exchange construct
        {
            value v1(object);
            value v2(array);
            value v3("Hello!");
            value(std::string("Hello!"));
            value(short{});
            value(int{});
            value(long{});
            value(long long{});
            value(unsigned short{});
            value(unsigned int{});
            value(unsigned long{});
            value(unsigned long long{});
            value(float{});
            value(double{});
            value(true);
            value(false);
            value v4(null);
            value(nullptr);
        }

        // exchange assign
        {
            value jv;
            jv = object;
            jv = array;
            jv = "Hello!";
            jv = std::string("Hello!");
            jv = short{};
            jv = int{};
            jv = long{};
            jv = long long{};
            jv = unsigned short{};
            jv = unsigned int{};
            jv = unsigned long{};
            jv = unsigned long long{};
            jv = float{};
            jv = double{};
            jv = true;
            jv = false;
            jv = null;
            jv = nullptr;
        }

        // raw
        {
            value jv;
            value const& jc(jv);
            {
                jv = object;
                BEAST_EXPECT(
                    jv.raw_object().size() == 0);
                jc.raw_object();
            }
            {
                jv = array;
                BEAST_EXPECT(
                    jv.raw_array().size() == 0);
                jc.raw_array();
            }
            {
                jv = "x";
                jv.raw_string() = "y";
                BEAST_EXPECT(jc.raw_string() == "y");
            }
            {
                jv = signed{};
                BEAST_EXPECT(jc.raw_signed() == 0);
                jv.raw_signed() = 1;
                BEAST_EXPECT(jc.raw_signed() == 1);
            }
            {
                jv = unsigned{};
                jv.raw_unsigned() = 2;
                BEAST_EXPECT(jc.raw_signed() == 2);
            }
            {
                jv = float{};
                jv.raw_floating() = 3;
                BEAST_EXPECT(jc.raw_floating() == 3);
            }
            {
                jv = bool{};
                jv.raw_bool() = true;
                BEAST_EXPECT(jc.raw_bool());
            }
        }
    }

    void run() override
    {
        testMembers();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,value);

} // json
} // beast
} // boost

