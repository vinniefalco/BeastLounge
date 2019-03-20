//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_VALUE_HPP
#define BOOST_BEAST_JSON_IMPL_VALUE_HPP

#include <boost/throw_exception.hpp>
#include <limits>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

namespace detail {

template<class T, class = void>
struct is_range : std::false_type
{
};

template<class T>
struct is_range<T, boost::void_t<
    typename T::value_type,
    decltype(
        std::declval<T const&>().begin(),
        std::declval<T const&>().end()
    )>> : std::true_type
{
};

} // detail

//------------------------------------------------------------------------------
//
// assign to value
//

// range
template<class T
    ,class = typename std::enable_if<
        detail::is_range<T>::value
        && ! std::is_same<typename T::value_type, char>::value
        && can_assign_to<typename T::value_type>::value
            >::type
>
void
assign(value& v, T const& t)
{
    v.reset(kind::array);
    for(auto const& e : t)
        v.as_array().push_back(e);
}

// string
inline
void
assign(value& v, string_view t)
{
    v.emplace_string().assign(
        t.data(), t.size());
}

// string
inline
void
assign(value& v, char const* t)
{
    v.emplace_string() = t;
}

// number
template<class T
    ,class = typename std::enable_if<
        std::is_constructible<number, T>::value &&
        ! std::is_same<number, T>::value>::type
>
inline
void
assign(value& v, T t)
{
    v.emplace_number() = t;
}

// bool
inline
void
assign(value& v, bool b)
{
    v.emplace_bool() = b;
}

// null
inline
void
assign(value& v, std::nullptr_t)
{
    v.reset(kind::null);
}

//------------------------------------------------------------------------------
//
// assign value to
//

// integer

template<typename T
    ,class = typename std::enable_if<
        std::is_integral<T>::value>::type
>
void
assign(T& t, value const& v, error_code& ec)
{
    if(v.is_int64())
    {
        auto const rhs = v.get_int64();
        if( rhs > (std::numeric_limits<T>::max)() ||
            rhs < (std::numeric_limits<T>::min)())
        {
            ec = error::integer_overflow;
            return;
        }
        t = static_cast<T>(rhs);
    }
    else if(v.is_uint64())
    {
        auto const rhs = v.get_uint64();
        if(rhs > (std::numeric_limits<T>::max)())
        {
            ec = error::integer_overflow;
            return;
        }
        t = static_cast<T>(rhs);
    }
    else
    {
        ec = error::expected_number;
        return;
    }
    ec = {};
}

} // json
} // beast
} // boost

#endif
