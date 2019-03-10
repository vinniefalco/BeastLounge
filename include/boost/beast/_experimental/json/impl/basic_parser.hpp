    //
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_BASIC_PARSER_HPP
#define BOOST_BEAST_JSON_IMPL_BASIC_PARSER_HPP

#include <boost/beast/core/buffers_range.hpp>

namespace boost {
namespace beast {
namespace json {

template<class ConstBufferSequence, class>
std::size_t
basic_parser::
write(ConstBufferSequence const& buffers, error_code& ec)
{
    static_assert(
        net::is_const_buffer_sequence<ConstBufferSequence>::value,
        "ConstBufferSequence type requirements not met");

    std::size_t bytes_used = 0;
    for(auto const b : beast::buffers_range_ref(buffers))
    {
        bytes_used += write(b, ec);
        if(ec)
            break;
    }
    return bytes_used;
}

} // json
} // beast
} // boost

#endif
