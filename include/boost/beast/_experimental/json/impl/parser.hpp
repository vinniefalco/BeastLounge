//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_PARSER_HPP
#define BOOST_BEAST_JSON_IMPL_PARSER_HPP

namespace boost {
namespace beast {
namespace json {

template<class T>
void
parser::
assign(T&& t)
{
    auto& jv = *stack_.front();
    if(jv.is_object())
    {
        BOOST_ASSERT(! key().empty());
        jv.raw_object().emplace(
            key(), std::forward<T>(t));
    }
    else if(stack_.front()->is_array())
    {
        BOOST_ASSERT(key().empty());
        jv.raw_array().emplace_back(
            std::forward<T>(t));
    }
    else
    {
        BOOST_ASSERT(jv.is_null());
        jv = std::forward<T>(t);
    }
}

} // json
} // beast
} // boost

#endif
