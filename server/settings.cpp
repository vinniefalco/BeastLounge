//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "settings.hpp"
#include <boost/beast/_experimental/json/parse_file.hpp>
#include <boost/beast/_experimental/json/parser.hpp>
#include <fstream>

std::shared_ptr<settings>
settings::
load_from_file(
    char const* path,
    beast::error_code& ec)
{
    {
        beast::json::parser p;
        beast::json::parse_file(path, p, ec);
    }
    std::shared_ptr<settings> sp;
    /*
    std::ifstream ifs;
    ifs.open(path);
    if(ifs.fail())
    {
        ec = make_error_code(
            beast::errc::no_such_file_or_directory);
        return nullptr;
    }
    ifs >> sp->json;
    if(ifs.fail())
    {
        ec = make_error_code(
            beast::errc::invalid_argument);
        return nullptr;
    }
    */
    return sp;
}
