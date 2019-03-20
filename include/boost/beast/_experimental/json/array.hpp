//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ARRAY_HPP
#define BOOST_BEAST_JSON_ARRAY_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/allocator.hpp>
#include <vector>

namespace boost {
namespace beast {
namespace json {

class value;

/** The native type of array values
*/
using array  =
    std::vector<
        value,
        allocator<value>>;

} // json
} // beast
} // boost

#endif
