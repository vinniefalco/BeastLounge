//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_KIND_HPP
#define BOOST_BEAST_JSON_KIND_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/detail/static_const.hpp>
#include <cstddef>

namespace boost {
namespace beast {
namespace json {

enum class kind
{
    object,
    array,
    string,
    signed64,
    unsigned64,
    floating,
    boolean,
    null
};

BOOST_BEAST_INLINE_VARIABLE(null, std::nullptr_t);

} // json
} // beast
} // boost

#endif
