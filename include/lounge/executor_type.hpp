//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_EXECUTOR_TYPE_HPP
#define LOUNGE_EXECUTOR_TYPE_HPP

#include <lounge/config.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>

namespace lounge {

/** The type of executor used for asynchronous I/O.
*/
using executor_type = net::strand<
    net::io_context::executor_type>;

} // lounge

#endif
