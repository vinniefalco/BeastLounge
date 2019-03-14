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
        v.raw_array().push_back(e);
}

inline
void
assign(value& v, string_view t)
{
    v.reset(kind::string);
    v.raw_string().assign(
        t.data(), t.size());
}

#if 0
template<std::size_t N>
void
assign(value& v, char const(&t)[N])
{
    v.reset(kind::string);
    v.raw_string().assign(t, N);
}
#else
inline
void
assign(value& v, char const* t)
{
    v.reset(kind::string);
    v.raw_string() = t;
}
#endif

inline
void
assign(value& v, short t)
{
    v.reset(kind::signed64);
    v.raw_signed() = t;
}

inline
void
assign(value& v, int t)
{
    v.reset(kind::signed64);
    v.raw_signed() = t;
}

inline
void
assign(value& v, long t)
{
    v.reset(kind::signed64);
    v.raw_signed() = t;
}

inline
void
assign(value& v, long long t)
{
    v.reset(kind::signed64);
    v.raw_signed() = t;
}

inline
void
assign(value& v, unsigned short t)
{
    v.reset(kind::unsigned64);
    v.raw_unsigned() = t;
}

inline
void
assign(value& v, unsigned int t)
{
    v.reset(kind::unsigned64);
    v.raw_unsigned() = t;
}

inline
void
assign(value& v, unsigned long t)
{
    v.reset(kind::unsigned64);
    v.raw_unsigned() = t;
}

inline
void
assign(value& v, unsigned long long t)
{
    v.reset(kind::unsigned64);
    v.raw_unsigned() = t;
}

inline
void
assign(value& v, float t)
{
    v.reset(kind::floating);
    v.raw_floating() = t;
}

inline
void
assign(value& v, double t)
{
    v.reset(kind::floating);
    v.raw_floating() = t;
}

inline
void
assign(value& v, bool t)
{
    v.reset(kind::boolean);
    v.raw_bool() = t;
}

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
    if(v.is_signed64())
    {
        auto const rhs = v.raw_signed();
        if( rhs > (std::numeric_limits<T>::max)() ||
            rhs < (std::numeric_limits<T>::min)())
        {
            ec = error::integer_overflow;
            return;
        }
        t = static_cast<T>(rhs);
    }
    else if(v.is_unsigned64())
    {
        auto const rhs = v.raw_unsigned();
        if(rhs > (std::numeric_limits<T>::max)())
        {
            ec = error::integer_overflow;
            return;
        }
        t = static_cast<T>(rhs);
    }
    else
    {
        ec = error::expected_unsigned;
        return;
    }
    ec = {};
}

} // json
} // beast
} // boost

#endif
