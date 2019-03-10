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
#include <unordered_map>
#include <vector>

namespace boost {
namespace beast {
namespace json {

class value;

using object_type =
    std::unordered_multimap<
        std::string,
        value,
        std::hash<std::string>,
        std::equal_to<std::string>,
        allocator<std::pair<
            std::string const, value>>>;

using array_type  =
    std::vector<
        value,
        allocator<value>>;

using string_type =
    std::basic_string<
        char,
        std::char_traits<char>,
        allocator<char>>;

using raw_object_type = object_type;
using raw_array_type = array_type;
using raw_string_type = string_type;
using raw_signed_type = std::int_least64_t;
using raw_unsigned_type = std::uint_least64_t;
using raw_floating_type = double;
using raw_bool_type = bool;

} // json
} // beast
} // boost

#endif
