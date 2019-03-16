//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_RPC_HPP
#define BOOST_BEAST_JSON_IMPL_RPC_HPP

#include <type_traits>

namespace boost {
namespace system {
template<>
struct is_error_code_enum<
    ::boost::beast::json::rpc_error>
{
    static bool constexpr value = true;
};
} // system
} // boost

namespace boost {
namespace beast {
namespace json {

BOOST_BEAST_DECL
error_code
make_error_code(rpc_error e);

} // json
} // beast
} // boost

#endif
