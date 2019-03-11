//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "settings.hpp"
#include <boost/beast/_experimental/json/assign_string.hpp>
#include <boost/beast/_experimental/json/assign_vector.hpp>
#include <boost/beast/_experimental/json/parse_file.hpp>
#include <boost/beast/_experimental/json/parser.hpp>
#include <fstream>

void
assign(
    settings::port& p,
    json::value const& jv,
    beast::error_code& ec)
{
    if(! jv.is_object())
    {
        ec = json::error::expected_object;
        return;
    }
    jv["name"].assign(p.name, ec);
    if(ec)
        return;
    jv["port"].assign(p.number, ec);
    if(ec)
        return;
    jv["address"].assign(p.addr, ec);
    if(ec)
        return;
}

namespace boost {
namespace beast {
namespace json {

template<>
struct value_exchange<net::ip::address>
{
    static
    void
    assign(
        net::ip::address& t,
        json::value const& jv,
        error_code& ec)
    {
        if(! jv.is_string())
        {
            ec = json::error::expected_string;
            return;
        }
        t = net::ip::make_address(jv.raw_string().c_str(), ec);
    }
};

} // json
} // beast
} // boost

std::shared_ptr<settings>
settings::
load_from_file(
    char const* path,
    beast::error_code& ec)
{
    json::parser p;
    json::parse_file(path, p, ec);
    if(ec)
        return nullptr;
    auto sp = std::make_shared<settings>();
    p.get()["ports"].assign(sp->ports, ec);
    if(ec)
        return nullptr;
    return sp;
}
