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
#include <vector>

namespace boost {
namespace beast {
namespace json {

class value_test : public unit_test::suite
{
public:     
    void
    testSpecial()
    {
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
    }

    void
    testConstruct()
    {
        // default ctor
        {
            value jv;
            BEAST_EXPECT(
                jv.is_null());
        }

        // storage ctor
        {
            storage_ptr sp =
                make_storage_ptr(
                    std::allocator<char>{});
            value jv1(sp);
            BEAST_EXPECT(jv1.is_null());
            value jv2(std::move(sp));
            BEAST_EXPECT(jv2.is_null());
        }

        // kind construct
        {
            value jv(kind::object);
            BEAST_EXPECT(
                jv.is_object());
        }
        {
            value jv(kind::array);
            BEAST_EXPECT(
                jv.is_array());
        }
        {
            value jv(kind::string);
            BEAST_EXPECT(
                jv.is_string());
        }
        {
            value jv(kind::signed64);
            BEAST_EXPECT(
                jv.is_signed64());
        }
        {
            value jv(kind::unsigned64);
            BEAST_EXPECT(
                jv.is_unsigned64());
        }
        {
            value jv(kind::floating);
            BEAST_EXPECT(
                jv.is_floating());
        }
        {
            value jv(kind::boolean);
            BEAST_EXPECT(
                jv.is_boolean());
        }
        {
            value jv(kind::null);
            BEAST_EXPECT(
                jv.is_null());
        }

        // kind, storage construct
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::object, sp);
            BEAST_EXPECT(
                jv.is_object());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::array, sp);
            BEAST_EXPECT(
                jv.is_array());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::string, sp);
            BEAST_EXPECT(
                jv.is_string());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::signed64, sp);
            BEAST_EXPECT(
                jv.is_signed64());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::unsigned64, sp);
            BEAST_EXPECT(
                jv.is_unsigned64());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::floating, sp);
            BEAST_EXPECT(
                jv.is_floating());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::boolean, sp);
            BEAST_EXPECT(
                jv.is_boolean());
        }
        {
            storage_ptr sp = make_storage_ptr(
                std::allocator<char>{});
            value jv(kind::null, sp);
            BEAST_EXPECT(
                jv.is_null());
        }

        // construct from containers
        {
            storage_ptr sp =
                make_storage_ptr(
                    std::allocator<char>());
            {
                object obj;
                value jv(obj);
                BEAST_EXPECT(jv.is_object());
            }
            {
                object obj;
                value jv(obj, sp);
                BEAST_EXPECT(jv.is_object());
            }
            {
                array arr;
                value jv(arr);
                BEAST_EXPECT(jv.is_array());
            }
            {
                array arr;
                value jv(arr, sp);
                BEAST_EXPECT(jv.is_array());
            }
            {
                string str;
                value jv(str);
                BEAST_EXPECT(jv.is_string());
            }
            {
                string str;
                value jv(str, sp);
                BEAST_EXPECT(jv.is_string());
            }
        }

        // assign from containers
        {
            {
                object obj;
                value jv;
                jv = obj;
                BEAST_EXPECT(jv.is_object());
                jv.reset();
                BEAST_EXPECT(jv.is_null());
                jv = std::move(obj);
                BEAST_EXPECT(jv.is_object());
            }
            {
                array arr;
                value jv;
                jv = arr;
                BEAST_EXPECT(jv.is_array());
                jv.reset();
                BEAST_EXPECT(jv.is_null());
                jv = std::move(arr);
                BEAST_EXPECT(jv.is_array());
            }
            {
                string str;
                value jv;
                jv = str;
                BEAST_EXPECT(jv.is_string());
                jv.reset();
                BEAST_EXPECT(jv.is_null());
                jv = std::move(str);
                BEAST_EXPECT(jv.is_string());
            }
        }
    }

    void
    testModifiers()
    {
        // reset
        {
            value jv;

            jv.reset(kind::object);
            BEAST_EXPECT(jv.is_object());

            jv.reset();
            BEAST_EXPECT(jv.is_null());

            jv.reset(kind::array);
            BEAST_EXPECT(jv.is_array());

            jv.reset(kind::string);
            BEAST_EXPECT(jv.is_string());

            jv.reset(kind::signed64);
            BEAST_EXPECT(jv.is_signed64());

            jv.reset(kind::unsigned64);
            BEAST_EXPECT(jv.is_unsigned64());

            jv.reset(kind::floating);
            BEAST_EXPECT(jv.is_floating());

            jv.reset(kind::boolean);
            BEAST_EXPECT(jv.is_boolean());

            jv.reset(kind::null);
            BEAST_EXPECT(jv.is_null());
        }

        // assign
        {
            value jv;

            jv = kind::object;
            BEAST_EXPECT(jv.is_object());

            jv = kind::null;
            BEAST_EXPECT(jv.is_null());

            jv = kind::array;
            BEAST_EXPECT(jv.is_array());

            jv = kind::string;
            BEAST_EXPECT(jv.is_string());

            jv = kind::signed64;
            BEAST_EXPECT(jv.is_signed64());

            jv = kind::unsigned64;
            BEAST_EXPECT(jv.is_unsigned64());

            jv = kind::floating;
            BEAST_EXPECT(jv.is_floating());

            jv = kind::boolean;
            BEAST_EXPECT(jv.is_boolean());

            jv = kind::null;
            BEAST_EXPECT(jv.is_null());
        }

        // emplace
        {
            {
                value jv;
                object& obj = jv.emplace_object();
                BEAST_EXPECT(jv.is_object());
                obj.clear();
            }
            {
                value jv;
                array& arr = jv.emplace_array();
                BEAST_EXPECT(jv.is_array());
                arr.clear();
            }
            {
                value jv;
                string& str = jv.emplace_string();
                BEAST_EXPECT(jv.is_string());
                str.clear();
            }
        }
    }

    void
    testExchange()
    {
        // construct from T
        {
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

        // assign from T
        {
            value jv;
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
    }

    void
    testRaw()
    {
        // raw
        value jv;
        value const& jc(jv);
        {
            jv = kind::object;
            BEAST_EXPECT(
                jv.raw_object().size() == 0);
            jc.raw_object();
        }
        {
            jv = kind::array;
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

    BOOST_STATIC_ASSERT(
        detail::is_range<std::vector<int>>::value);

    BOOST_STATIC_ASSERT(
        detail::is_range<std::initializer_list<int>>::value);

    void run() override
    {
        log <<
            "sizeof(value) == " <<
            sizeof(value) << "\n";
        log <<
            "sizeof(object) == " <<
            sizeof(object) << "\n";
        log <<
            "sizeof(array) == " <<
            sizeof(array) << "\n";
        log <<
            "sizeof(string) == " <<
            sizeof(string) << "\n";
        log <<
            "sizeof(value::native) == " <<
            sizeof(value::native) << "\n";
        testSpecial();
        testConstruct();
        testModifiers();
        testExchange();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,value);

} // json
} // beast
} // boost

