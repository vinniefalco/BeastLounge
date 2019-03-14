//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_KEY_PARAM_HPP
#define BOOST_BEAST_JSON_KEY_PARAM_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/string.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

/** Customization point for mapping enums to key strings.

    To use this, specialize the class for the enum type and
    provide this function signature in the same namespace
    as the enumeration:

    @code
    string_view make_key_string(T)
    @endcode
*/
template<class T>
struct is_key_enum : std::false_type
{
};

/** A type passed to functions expecting a key.
*/
class key_param
{
    string_view str_;

public:
    key_param() = default;
    key_param(key_param const&) = default;
    key_param& operator=(key_param const&) = default;

    template<class T
#ifndef BOOST_DOXYGEN
        ,class = typename std::enable_if<
            std::is_constructible<
                string_view, T>::value>::type
#endif
    >
    key_param(T&& t)
        : str_(t)
    {
    }

    template<class KeyEnum
#ifndef BOOST_DOXYGEN
        ,class = typename std::enable_if<
            is_key_enum<KeyEnum>::value>::type
#endif
    >
    key_param(KeyEnum e)
        : str_(make_key_string(e))
    {
    }

    /** Return the key as a string view
    */
    string_view
    str() const noexcept
    {
        return str_;
    }

    /** Convert to string view
    */
    operator string_view() const noexcept
    {
        return str_;
    }
};

inline
bool
operator==(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() == rhs.str();
}

inline
bool
operator!=(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() != rhs.str();
}

inline
bool
operator<(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() < rhs.str();
}

inline
bool
operator<=(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() <= rhs.str();
}

inline
bool
operator>(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() > rhs.str();
}

inline
bool
operator>=(key_param lhs, key_param rhs) noexcept
{
    return lhs.str() >= rhs.str();
}

} // json
} // beast
} // boost

#endif
