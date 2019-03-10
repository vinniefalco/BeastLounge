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
#include <boost/beast/_experimental/json/detail/access.hpp>
#include <cstddef>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

template<>
struct exchange<object_t> : detail::access
{
    static void assign(value& jv, object_t)
    {
        impl(jv).set_kind(kind::object);
    }
};

template<>
struct exchange<array_t> : detail::access
{
    static void assign(value& jv, array_t)
    {
        impl(jv).set_kind(kind::array);
    }
};

//
// strings
//

template<std::size_t N>
struct exchange<char[N]> : detail::access
{
    static void assign(value& jv, string_view v)
    {
        impl(jv).set_kind(kind::string);
        jv.raw_string().assign(
            v.data(), v.size());
    }
};

template<class Traits, class Allocator>
struct exchange<
    std::basic_string<char, Traits, Allocator>>
    : detail::access
{
    static void assign(value& jv,
        std::basic_string<char, Traits, Allocator> const& v)
    {
        impl(jv).set_kind(kind::string);
        jv.raw_string().assign(
            v.data(), v.size());
    }
};

//
// signed integers
//

template<>
struct exchange<short> : detail::access
{
    static void assign(value& jv, short v)
    {
        impl(jv).set_kind(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<int> : detail::access
{
    static void assign(value& jv, int v)
    {
        impl(jv).set_kind(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<long> : detail::access
{
    static void assign(value& jv, long v)
    {
        impl(jv).set_kind(kind::signed64);
        jv.raw_signed() = v;
    }
};

template<>
struct exchange<long long> : detail::access
{
    static void assign(value& jv, long long v)
    {
        impl(jv).set_kind(kind::signed64);
        jv.raw_signed() = v;
    }
};

//
// unsigned integers
//

template<>
struct exchange<unsigned short> : detail::access
{
    static void assign(value& jv, unsigned short v)
    {
        impl(jv).set_kind(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned int> : detail::access
{
    static void assign(value& jv, unsigned int v)
    {
        impl(jv).set_kind(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned long> : detail::access
{
    static void assign(value& jv, unsigned long v)
    {
        impl(jv).set_kind(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

template<>
struct exchange<unsigned long long> : detail::access
{
    static void assign(value& jv, unsigned long long v)
    {
        impl(jv).set_kind(kind::unsigned64);
        jv.raw_unsigned() = v;
    }
};

//
// floating point
//

template<>
struct exchange<float> : detail::access
{
    static void assign(value& jv, float v)
    {
        impl(jv).set_kind(kind::floating);
        jv.raw_floating() = v;
    }
};

template<>
struct exchange<double> : detail::access
{
    static void assign(value& jv, double v)
    {
        impl(jv).set_kind(kind::floating);
        jv.raw_floating() = v;
    }
};

//
// boolean
//

template<>
struct exchange<bool> : detail::access
{
    static void assign(value& jv, bool v)
    {
        impl(jv).set_kind(kind::boolean);
        jv.raw_bool() = v;
    }
};

//
// null
//

template<>
struct exchange<std::nullptr_t> : detail::access
{
    static void assign(value& jv, std::nullptr_t)
    {
        impl(jv).set_kind(kind::null);
    }
};

} // json
} // beast
} // boost

#endif
