//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

// Test that header file is self-contained.
#include <boost/beast/_experimental/json/object.hpp>

#include <boost/beast/_experimental/unit_test/suite.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class object_test : public unit_test::suite
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
    check(
        object const& obj,
        std::size_t bucket_count)
    {
        BEAST_EXPECT(! obj.empty());
        BEAST_EXPECT(obj.size() == 3);
        BEAST_EXPECT(
            obj.bucket_count() == bucket_count);
        BEAST_EXPECT(
            obj["a"].raw_signed() == 1);
        BEAST_EXPECT(
            obj["b"].raw_bool());
        BEAST_EXPECT(
            obj["c"].raw_string() == "hello");

        // ordering, storage

        auto it = obj.begin();
        BEAST_EXPECT(it->first == "a");
        BEAST_EXPECT(
            *it->second.get_storage() ==
            *obj.get_storage());

        ++it;
        BEAST_EXPECT(it->first == "b");
        BEAST_EXPECT(
            *it->second.get_storage() ==
            *obj.get_storage());

        it++;
        BEAST_EXPECT(it->first == "c");
        BEAST_EXPECT(
            *it->second.get_storage() ==
            *obj.get_storage());
    }

    void
    testSpecial()
    {
        unique_storage us;
        storage_ptr sp(&us);
        storage_ptr sp0 =
            get_default_storage_ptr();
        BEAST_EXPECT(*sp != *sp0);

        // basic ctors
        {
            object obj;
            BEAST_EXPECT(obj.empty());
            BEAST_EXPECT(obj.size() == 0);
            BEAST_EXPECT(
                obj.bucket_count() == 0);
            BEAST_EXPECT(
                *obj.get_storage() == *sp0);
        }
        {
            object obj(50);
            BEAST_EXPECT(obj.empty());
            BEAST_EXPECT(obj.size() == 0);
            BEAST_EXPECT(obj.bucket_count() == 53);
            BEAST_EXPECT(
                *obj.get_storage() == *sp0);
        }
        {
            object obj(sp);
            BEAST_EXPECT(
                *obj.get_storage() == *sp);
        }
        {
            object obj(50, sp);
            BEAST_EXPECT(obj.empty());
            BEAST_EXPECT(obj.size() == 0);
            BEAST_EXPECT(obj.bucket_count() == 53);
            BEAST_EXPECT(
                *obj.get_storage() == *sp);
        }
       
        // iterator ctors
        {
            std::initializer_list<std::pair<
                beast::string_view, value>> init = {
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}};
            {
                object obj(init.begin(), init.end());
                check(obj, 3);
            }
            {
                object obj(init.begin(), init.end(), 5);
                check(obj, 7);
            }
            {
                object obj(init.begin(), init.end(), sp);
                BEAST_EXPECT(
                    *obj.get_storage() == *sp);
                check(obj, 3);
            }
            {
                object obj(init.begin(), init.end(), 5, sp);
                BEAST_EXPECT(! obj.empty());
                BEAST_EXPECT(obj.size() == 3);
                BEAST_EXPECT(obj.bucket_count() == 7);
                BEAST_EXPECT(
                    *obj.get_storage() == *sp);
                check(obj, 7);
            }
        }
        
        // move/copy ctors
        {
            {
                object obj1({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    });
                object obj2(std::move(obj1));
                BEAST_EXPECT(
                    *obj1.get_storage() ==
                    *obj2.get_storage());
                BEAST_EXPECT(obj1.empty());
                BEAST_EXPECT(obj1.size() == 0);
                check(obj2, 3);
            }
            {
                object obj1({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    });
                object obj2(std::move(obj1), sp);
                BEAST_EXPECT(obj1.empty());
                BEAST_EXPECT(
                    *obj1.get_storage() !=
                    *obj2.get_storage());
                check(obj2, 3);
            }
            {

                object obj1({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    });
                object obj2(obj1);
                BEAST_EXPECT(
                    *obj1.get_storage() ==
                    *obj2.get_storage());
                check(obj2, 3);
            }
            {

                object obj1({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    });
                object obj2(obj1, sp);
                BEAST_EXPECT(
                    *obj1.get_storage() !=
                    *obj2.get_storage());
                check(obj2, 3);
            }
        }

        // init-list ctor
        {
            {
                object obj({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    });
                check(obj, 3);
            }
            {
                object obj({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    },
                    5);
                check(obj, 7);
            }
            {
                object obj({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    },
                    sp);
                BEAST_EXPECT(
                    *obj.get_storage() == *sp);
                check(obj, 3);
            }
            {
                object obj({
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}
                    },
                    5, sp);
                BEAST_EXPECT(
                    *obj.get_storage() == *sp);
                check(obj, 7);
            }
        }

        // assignment
        {
            {
                object obj;

                obj = {
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"} },
                check(obj, 3);
                BEAST_EXPECT(
                    *obj.get_storage() == *sp0);
            }
            {
                object obj(sp);

                obj = {
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"} },
                BEAST_EXPECT(
                    *obj.get_storage() == *sp);
                check(obj, 3);
            }
        }
    }

    void
    testIterators()
    {
        object obj({
            {"a", 1},
            {"b", true},
            {"c", "hello"}});
        {
            auto it = obj.begin();
            BEAST_EXPECT(it->first == "a"); ++it;
            BEAST_EXPECT(it->first == "b"); it++;
            BEAST_EXPECT(it->first == "c"); ++it;
            BEAST_EXPECT(it == obj.end());
        }
        {
            auto it = obj.cbegin();
            BEAST_EXPECT(it->first == "a"); ++it;
            BEAST_EXPECT(it->first == "b"); it++;
            BEAST_EXPECT(it->first == "c"); ++it;
            BEAST_EXPECT(it == obj.cend());
        }
        {
            auto it = static_cast<
                object const&>(obj).begin();
            BEAST_EXPECT(it->first == "a"); ++it;
            BEAST_EXPECT(it->first == "b"); it++;
            BEAST_EXPECT(it->first == "c"); ++it;
            BEAST_EXPECT(it == static_cast<
                object const&>(obj).end());
        }
        {
            auto it = obj.end();
            --it; BEAST_EXPECT(it->first == "c");
            it--; BEAST_EXPECT(it->first == "b");
            --it; BEAST_EXPECT(it->first == "a");
            BEAST_EXPECT(it == obj.begin());
        }
        {
            auto it = obj.cend();
            --it; BEAST_EXPECT(it->first == "c");
            it--; BEAST_EXPECT(it->first == "b");
            --it; BEAST_EXPECT(it->first == "a");
            BEAST_EXPECT(it == obj.cbegin());
        }
        {
            auto it = static_cast<
                object const&>(obj).end();
            --it; BEAST_EXPECT(it->first == "c");
            it--; BEAST_EXPECT(it->first == "b");
            --it; BEAST_EXPECT(it->first == "a");
            BEAST_EXPECT(it == static_cast<
                object const&>(obj).begin());
        }
    }

    void
    testModifiers()
    {
        // insert
        {
            object obj;
            auto p = object::value_type("a", 1);
            auto result = obj.insert(std::move(p));
            BEAST_EXPECT(p.second.is_null());
            BEAST_EXPECT(result.second);
            BEAST_EXPECT(
                result.first->first == "a");
            BEAST_EXPECT(
                ! obj.insert({"a", 2}).second);
        }
        {
            object obj;
            auto p = object::value_type("a", 1);
            auto result = obj.insert(p);
            BEAST_EXPECT(! p.second.is_null());
            BEAST_EXPECT(result.second);
            BEAST_EXPECT(
                result.first->first == "a");
            BEAST_EXPECT(
                ! obj.insert({"a", 2}).second);
        }

        // insert P
        // TODO

        // insert hint
        {
            object obj;
            auto hint = obj.end();
            auto p = object::value_type("a", 1);
            auto it =
                obj.insert(hint, std::move(p));
            BEAST_EXPECT(it != obj.end());
            BEAST_EXPECT(p.second.is_null());
            BEAST_EXPECT(it->first == "a");
            BEAST_EXPECT(
                ! obj.insert({"a", 2}).second);
        }
        {
            object obj;
            auto hint = obj.end();
            auto p = object::value_type("a", 1);
            auto it = obj.insert(hint, p);
            BEAST_EXPECT(it != obj.end());
            BEAST_EXPECT(! p.second.is_null());
            BEAST_EXPECT(it->first == "a");
            BEAST_EXPECT(
                ! obj.insert({"a", 2}).second);
        }

        // insert P hint
        // TODO

        // insert range
        {
            std::initializer_list<std::pair<
                beast::string_view, value>> init = {
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}};
            object obj;
            obj.insert(init.begin(), init.end());
            check(obj, 3);
        }
        {
            std::initializer_list<std::pair<
                beast::string_view, value>> init = {
                    {"a", 1},
                    {"b", true},
                    {"c", "hello"}};
            object obj;
            obj.insert(init);
            check(obj, 3);
        }
    }

    void
    testRehash()
    {
        object obj;
        for(std::size_t i = 0; i < 1000; ++i)
            obj.emplace(
                std::to_string(i),
                i);
    }

    void
    run() override
    {
        testSpecial();
        testIterators();
        testModifiers();
        testRehash();
    }
};

BEAST_DEFINE_TESTSUITE(beast,json,object);

} // json
} // beast
} // boost
