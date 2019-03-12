//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_CONFIG_HPP
#define LOUNGE_CONFIG_HPP

// This file must be included directly or indirectly in every source file.

namespace boost {

namespace asio {
namespace ip {
class tcp;
}
}

namespace beast {
namespace http {}
namespace json {}
namespace websocket {}
namespace ssl {}
} // beast

} // boost

namespace net   = boost::asio;
namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace json  = boost::beast::json;
namespace http  = boost::beast::http;
namespace websocket = boost::beast::websocket;

using tcp = net::ip::tcp;

// Define this on to use the system executor
//#define LOUNGE_USE_SYSTEM_EXECUTOR

#endif
