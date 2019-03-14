//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_TYPES_HPP
#define BOOST_BEAST_JSON_TYPES_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/allocator.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace boost {
namespace beast {
namespace json {

class value;

using array  =
    std::vector<
        value,
        allocator<value>>;

using string =
    std::basic_string<
        char,
        std::char_traits<char>,
        allocator<char>>;

using signed64 = std::int_least64_t;

using unsigned64 = std::uint_least64_t;

using floating = double;

using boolean = bool;

} // json
} // beast
} // boost

#endif
