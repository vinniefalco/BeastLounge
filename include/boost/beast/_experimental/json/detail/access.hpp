//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_DETAIL_ACCESS_HPP
#define BOOST_BEAST_JSON_DETAIL_ACCESS_HPP

#include <boost/beast/_experimental/json/value.hpp>

namespace boost {
namespace beast {
namespace json {
namespace detail {

class access
{
protected:
    static
    detail::value_impl&
    impl(value& v) noexcept
    {
        return v.impl_;
    }
};

} // detail
} // json
} // beast
} // boost

#endif
