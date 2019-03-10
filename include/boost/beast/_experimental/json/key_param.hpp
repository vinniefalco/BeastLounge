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

/** A type passed to functions expecting a key.
*/
struct key_param
{
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
        : str(t)
    {
    }

    template<class Enum
#ifndef BOOST_DOXYGEN
        ,class = typename std::enable_if<
            std::is_enum<Enum>::value>::type
#endif
    >
    key_param(Enum e)
    {
        // TODO
        str = "enum";
    }

    string_view str;
};

} // json
} // beast
} // boost

#endif
