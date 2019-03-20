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
    struct unique_storage : storage
    {
        void
        addref() noexcept override
        {
        }

        void
        release() noexcept override
        {
        }

        void*
        allocate(
            std::size_t n,
            std::size_t) override
        {
            return std::allocator<
                char>{}.allocate(n);
        }

        void
        deallocate(
            void* p,
            std::size_t n,
            std::size_t) noexcept override
        {
            auto cp =
                reinterpret_cast<char*>(p);
            return std::allocator<
                char>{}.deallocate(cp, n);
        }
        bool
        is_equal(
            storage const& other
                ) const noexcept override
        {
            auto p = dynamic_cast<
                unique_storage const*>(&other);
            if(! p)
                return false;
            return this == p;
        }
    };

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
        unique_storage us;
        storage_ptr sp(&us);
        storage_ptr sp0 =
            get_default_storage_ptr();

        // default ctor
        {
            value jv;
            BEAST_EXPECT(jv.is_null());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }

        // storage ctor
        {
            value jv1(sp);
            BEAST_EXPECT(jv1.is_null());
            BEAST_EXPECT(jv1.get_storage() == sp);
            BEAST_EXPECT(jv1.get_storage() != sp0);

            auto sp2 = sp;
            value jv2(std::move(sp2));
            BEAST_EXPECT(jv2.is_null());
            BEAST_EXPECT(jv1.get_storage() == sp);
            BEAST_EXPECT(jv1.get_storage() != sp0);
            BEAST_EXPECT(jv2.get_storage() == sp);
            BEAST_EXPECT(jv2.get_storage() != sp0);
        }

        // kind construct
        {
            value jv(kind::object);
            BEAST_EXPECT(jv.is_object());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }
        {
            value jv(kind::array);
            BEAST_EXPECT(jv.is_array());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }
        {
            value jv(kind::string);
            BEAST_EXPECT(jv.is_string());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }
        {
            value jv(kind::number);
            BEAST_EXPECT(jv.is_number());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }
        {
            value jv(kind::boolean);
            BEAST_EXPECT(jv.is_bool());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }
        {
            value jv(kind::null);
            BEAST_EXPECT(jv.is_null());
            BEAST_EXPECT(jv.get_storage() != sp);
            BEAST_EXPECT(jv.get_storage() == sp0);
        }

        // kind, storage construct
        {
            value jv(kind::object, sp);
            BEAST_EXPECT(jv.is_object());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }
        {
            value jv(kind::array, sp);
            BEAST_EXPECT(jv.is_array());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }
        {
            value jv(kind::string, sp);
            BEAST_EXPECT(jv.is_string());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }
        {
            value jv(kind::number, sp);
            BEAST_EXPECT(jv.is_number());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }
        {
            value jv(kind::boolean, sp);
            BEAST_EXPECT(jv.is_bool());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }
        {
            value jv(kind::null, sp);
            BEAST_EXPECT(jv.is_null());
            BEAST_EXPECT(jv.get_storage() == sp);
            BEAST_EXPECT(jv.get_storage() != sp0);
        }

        // construct from containers
        {
            {
                object obj;
                value jv(obj);
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                array arr;
                value jv(arr);
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                string str;
                value jv(str);
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                value jv(number(1));
                BEAST_EXPECT(jv.is_number());
                BEAST_EXPECT(jv.is_int64());
                BEAST_EXPECT(jv.is_uint64());
                BEAST_EXPECT(jv.is_double());
                BEAST_EXPECT(jv.get_int64() == 1);
                BEAST_EXPECT(jv.get_uint64() == 1);
                BEAST_EXPECT(jv.get_double() == 1);
                BEAST_EXPECT(jv.as_number() == 1);
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            //
            {
                object obj;
                value jv(obj, sp);
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                array arr;
                value jv(arr, sp);
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                string str;
                value jv(str, sp);
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                value jv(number(1), sp);
                BEAST_EXPECT(jv.is_number());
                BEAST_EXPECT(jv.is_int64());
                BEAST_EXPECT(jv.is_uint64());
                BEAST_EXPECT(jv.is_double());
                BEAST_EXPECT(jv.get_int64() == 1);
                BEAST_EXPECT(jv.get_uint64() == 1);
                BEAST_EXPECT(jv.get_double() == 1);
                BEAST_EXPECT(jv.as_number() == 1);
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
        }

        // assign from containers
        {
            {
                object obj;
                value jv;

                jv = obj;
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv = std::move(obj);
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                object obj;
                value jv(sp);

                jv = obj;
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv = std::move(obj);
                BEAST_EXPECT(jv.is_object());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                array arr;
                value jv;

                jv = arr;
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv = std::move(arr);
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                array arr;
                value jv(sp);

                jv = arr;
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv = std::move(arr);
                BEAST_EXPECT(jv.is_array());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                string str;
                value jv;

                jv = str;
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);

                jv = std::move(str);
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                string str;
                value jv(sp);

                jv = str;
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv.reset();
                BEAST_EXPECT(jv.is_null());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);

                jv = std::move(str);
                BEAST_EXPECT(jv.is_string());
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
            }
            {
                number n(1);
                value jv;
                jv = n;
                BEAST_EXPECT(jv.is_number());
                BEAST_EXPECT(jv.is_int64());
                BEAST_EXPECT(jv.is_uint64());
                BEAST_EXPECT(jv.is_double());
                BEAST_EXPECT(jv.get_int64() == 1);
                BEAST_EXPECT(jv.get_uint64() == 1);
                BEAST_EXPECT(jv.get_double() == 1);
                BEAST_EXPECT(jv.as_number() == 1);
                BEAST_EXPECT(jv.get_storage() != sp);
                BEAST_EXPECT(jv.get_storage() == sp0);
            }
            {
                number n(1);
                value jv(sp);
                jv = n;
                BEAST_EXPECT(jv.is_number());
                BEAST_EXPECT(jv.is_int64());
                BEAST_EXPECT(jv.is_uint64());
                BEAST_EXPECT(jv.is_double());
                BEAST_EXPECT(jv.get_int64() == 1);
                BEAST_EXPECT(jv.get_uint64() == 1);
                BEAST_EXPECT(jv.get_double() == 1);
                BEAST_EXPECT(jv.as_number() == 1);
                BEAST_EXPECT(jv.get_storage() == sp);
                BEAST_EXPECT(jv.get_storage() != sp0);
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

            jv.reset(kind::number);
            BEAST_EXPECT(jv.is_number());

            jv.reset(kind::boolean);
            BEAST_EXPECT(jv.is_bool());

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

            jv = kind::number;
            BEAST_EXPECT(jv.is_number());

            jv = kind::boolean;
            BEAST_EXPECT(jv.is_bool());

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
            {
                value jv;
                number& n= jv.emplace_number();
                BEAST_EXPECT(jv.is_number());
                n = 0;
            }
            {
                value jv;
                bool& b= jv.emplace_bool();
                BEAST_EXPECT(jv.is_bool());
                b = false;
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
            value(long double{});
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
            jv = long double{};
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
                jv.as_object().size() == 0);
            jc.as_object();
        }
        {
            jv = kind::array;
            BEAST_EXPECT(
                jv.as_array().size() == 0);
            jc.as_array();
        }
        {
            jv = "x";
            jv.as_string() = "y";
            BEAST_EXPECT(jc.as_string() == "y");
        }
        {
            jv = signed{};
            BEAST_EXPECT(jc.get_int64() == 0);
            jv.as_number() = -1;
            BEAST_EXPECT(jc.get_int64() == -1);
        }
        {
            jv = unsigned{};
            jv.as_number() = 2;
            BEAST_EXPECT(jc.get_uint64() == 2);
        }
        {
            jv = bool{};
            jv.as_bool() = true;
            BEAST_EXPECT(jc.as_bool());
        }
    }

    BOOST_STATIC_ASSERT(
        detail::is_range<std::vector<int>>::value);

    BOOST_STATIC_ASSERT(
        detail::is_range<std::initializer_list<int>>::value);

    void run() override
    {
        log <<
            "sizeof(value)  == " <<
            sizeof(value) << "\n";
        log <<
            "sizeof(object) == " <<
            sizeof(object) << "\n";
        log <<
            "sizeof(array)  == " <<
            sizeof(array) << "\n";
        log <<
            "sizeof(string) == " <<
            sizeof(string) << "\n";
        log <<
            "sizeof(number) == " <<
            sizeof(number) << "\n";
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

