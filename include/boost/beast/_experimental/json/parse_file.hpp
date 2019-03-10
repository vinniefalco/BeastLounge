//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_PARSE_FILE_HPP
#define BOOST_BEAST_JSON_PARSE_FILE_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/basic_parser.hpp>
#include <boost/beast/core/error.hpp>

namespace boost {
namespace beast {
namespace json {

/** Parse the contents of a file as JSON.
*/
BOOST_BEAST_DECL
void
parse_file(
    char const* path,
    basic_parser& parser,
    error_code& ec);

} // json
} // beast
} // boost

#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/parse_file.ipp>
#endif

#endif
