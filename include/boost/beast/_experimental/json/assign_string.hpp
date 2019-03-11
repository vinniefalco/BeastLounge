//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ASSIGN_STRING_HPP
#define BOOST_BEAST_JSON_ASSIGN_STRING_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/beast/_experimental/json/value.hpp>
#include <string>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

template<class Allocator>
void
assign(
    std::basic_string<
        char,
        std::char_traits<char>,
        Allocator>& t,
    value const& v,
    error_code& ec)
{
    if(! v.is_string())
    {
        ec = error::expected_string;
        return;
    }
    auto& s= v.raw_string();
    t.assign(s.data(), s.size());
}

} // json
} // beast
} // boost

#endif
