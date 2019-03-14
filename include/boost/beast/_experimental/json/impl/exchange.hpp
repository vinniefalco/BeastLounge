//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_DETAIL_EXCHANGE_HPP
#define BOOST_BEAST_JSON_DETAIL_EXCHANGE_HPP

#include <boost/beast/_experimental/json/exchange.hpp>
#include <cstddef>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

//
// strings
//

template<std::size_t N>
struct exchange<char[N]>
{
    static void assign(value& jv, string_view v)
    {
        jv.reset(kind::string);
        jv.raw_string().assign(
            v.data(), v.size());
    }
};

template<class Traits, class Allocator>
struct exchange<
    std::basic_string<char, Traits, Allocator>>
   
{
    static void assign(value& jv,
        std::basic_string<char, Traits, Allocator> const& v)
    {
        jv.reset(kind::string);
        jv.raw_string().assign(
            v.data(), v.size());
    }
};

//
// signed integers
//

template<>
struct exchange<short>
{
    static void assign(value& jv, short v)
    {
        jv.reset(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<int>
{
    static void assign(value& jv, int v)
    {
        jv.reset(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<long>
{
    static void assign(value& jv, long v)
    {
        jv.reset(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<long long>
{
    static void assign(value& jv, long long v)
    {
        jv.reset(kind::signed64);
        jv.raw_signed() = v;
    }
};

//
// unsigned integers
//

template<>
struct exchange<unsigned short>
{
    static void assign(value& jv, unsigned short v)
    {
        jv.reset(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned int>
{
    static void assign(value& jv, unsigned int v)
    {
        jv.reset(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned long>
{
    static void assign(value& jv, unsigned long v)
    {
        jv.reset(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned long long>
{
    static void assign(value& jv, unsigned long long v)
    {
        jv.reset(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

//
// floating point
//

template<>
struct exchange<float>
{
    static void assign(value& jv, float v)
    {
        jv.reset(kind::floating);
        jv.raw_floating() = v;
    }
};

template<>
struct exchange<double>
{
    static void assign(value& jv, double v)
    {
        jv.reset(kind::floating);
        jv.raw_floating() = v;
    }
};

//
// boolean
//

template<>
struct exchange<bool>
{
    static void assign(value& jv, bool v)
    {
        jv.reset(kind::boolean);
        jv.raw_bool() = v;
    }
};

//
// null
//

template<>
struct exchange<std::nullptr_t>
{
    static void assign(value& jv, std::nullptr_t)
    {
        jv.reset(kind::null);
    }
};

} // json
} // beast
} // boost

#endif
