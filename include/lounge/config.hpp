//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CONFIG_HPP
#define LOUNGE_CONFIG_HPP

#include <lounge/detail/config.hpp>
#include <boost/system/error_code.hpp>
#include <boost/utility/string_view.hpp>

// This file must be included directly or indirectly in every source file.

namespace boost {
namespace asio {
namespace ip {
class tcp;
} // ip
} // asio
} // boost

namespace boost {
namespace json {
class value;
} // json
} // boost

namespace boost {
namespace beast {
namespace http {}
namespace websocket {}
namespace ssl {}
} // beast
} // boost

namespace lounge {

namespace net   = boost::asio;
namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace json  = boost::json;
namespace http  = boost::beast::http;
namespace websocket = boost::beast::websocket;

using tcp = net::ip::tcp;

using string_view = boost::string_view;

using error_code = boost::system::error_code;
using error_category = boost::system::error_category;
using error_condition = boost::system::error_condition;

} // lounge

#endif
