//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SETTINGS_HPP
#define LOUNGE_SETTINGS_HPP

#include "config.hpp"
#include <boost/beast/core/error.hpp>
#include <boost/asio/ip/address.hpp>
#include <memory>
#include <vector>

struct settings
{
    struct port
    {
        std::string name;       // name of this port
        net::ip::address addr;  // address to bind to
        unsigned short number;  // port number
    };

    std::vector<port> ports;

    static
    std::shared_ptr<settings>
    load_from_file(
        char const* path,
        beast::error_code& ec);
};

#endif
