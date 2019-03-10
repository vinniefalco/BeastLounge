//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_EXCHANGE_HPP
#define BOOST_BEAST_JSON_EXCHANGE_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/detail/unspecialized.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

/** Customization point for converting to and from JSON.
*/
template<class Value>
struct exchange final
#ifndef BOOST_BEAST_DOXYGEN
    : detail::unspecialized
#endif
{
};

/** Trait to determine if a type can be exchanged with JSON
*/
template<class T>
#ifdef BOOST_BEAST_DOXYGEN
using is_exchange_type = __see_below__;
#else
using is_exchange_type =
    std::integral_constant<bool, ! std::is_base_of<
        detail::unspecialized, exchange<
            typename std::remove_const<
            typename std::remove_reference<T>::type
                >::type> >::value>;
#endif

} // json
} // beast
} // boost

#endif
