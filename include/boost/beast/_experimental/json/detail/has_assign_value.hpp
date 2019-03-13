//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_DETAIL_HAS_ASSIGN_VALUE_HPP
#define BOOST_BEAST_JSON_DETAIL_HAS_ASSIGN_VALUE_HPP

#include <boost/beast/core/error.hpp>
#include <boost/type_traits/make_void.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class value;

namespace detail {

// Determine if `assign(value&,T)` is found via ADL
template<class T, class = void>
struct has_adl_value_from : std::false_type
{
};

template<class T>
struct has_adl_value_from<T,
    boost::void_t<decltype(assign(
        std::declval<json::value&>(),
        std::declval<T>()))>>
    : std::true_type
{
};

// Determine if `error_code assign(T&,value)` is found via ADL
template<class T, class = void>
struct has_adl_value_to : std::false_type
{
};

template<class T>
struct has_adl_value_to<T,
    boost::void_t<decltype(assign(
        std::declval<T&>(),
        std::declval<json::value const&>(),
        std::declval<error_code&>()))>>
    : std::true_type
{
};

// Determine if `t.assign(value&)` exists
template<class T, class = void>
struct has_mf_value_from : std::false_type
{
};

template<class T>
struct has_mf_value_from<T,
    boost::void_t<decltype(
        std::declval<T const&>().assign(
            std::declval<json::value&>()))>>
    : std::true_type
{
};

// Determine if `t.assign(value const&,error_code&)` exists
template<class T, class = void>
struct has_mf_value_to : std::false_type
{
};

template<class T>
struct has_mf_value_to<T,
    boost::void_t<decltype(
    std::declval<T&>().assign(
        std::declval<json::value const&>(),
        std::declval<error_code&>()))>>
    : std::true_type
{
};

template<class T>
void
call_assign(
    value& v,
    T const& t,
    std::true_type)
{
    t.assign(v);
}

template<class T>
void
call_assign(
    value& v,
    T const& t,
    std::false_type)
{
    assign(v, t);
}

template<class T>
void
call_assign(
    value& v,
    T const& t)
{
    call_assign(v, t,
        has_mf_value_from<T>{});
}

template<class T>
void
call_assign(
    T& t,
    value const& v,
    error_code& ec,
    std::true_type)
{
    t.assign(v, ec);
}

template<class T>
void
call_assign(
    T& t,
    value const& v,
    error_code& ec,
    std::false_type)
{
    assign(t, v, ec);
}

template<class T>
void
call_assign(
    T& t,
    value const& v,
    error_code& ec)
{
    call_assign(t, v, ec,
        has_mf_value_to<T>{});
}

} // detail
} // json
} // beast
} // boost

#endif
