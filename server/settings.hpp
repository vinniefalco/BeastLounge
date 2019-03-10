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
#include <memory>

struct settings
{
    static
    std::shared_ptr<settings>
    load_from_file(
        char const* path,
        beast::error_code& ec);
};

#endif
