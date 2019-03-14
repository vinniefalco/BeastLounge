//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_PARSER_IPP
#define BOOST_BEAST_JSON_IMPL_PARSER_IPP

#include <boost/beast/_experimental/json/parser.hpp>
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace beast {
namespace json {

parser::
parser()
{
}

parser::
parser(storage_ptr const& store)
    : jv_(store)
{
}

value const&
parser::
get() const noexcept
{
    return jv_;
}

value
parser::
release() noexcept
{
    return std::move(jv_);
}

void
parser::
on_document_begin(error_code&)
{
    stack_.clear();
    stack_.push_front(&jv_);
    s_.clear();
}

void
parser::
on_object_begin(error_code& ec)
{
    if(stack_.size() >= max_depth_)
    {
        ec = error::too_deep;
        return;
    }
    auto& jv = *stack_.front();
    if(jv.is_object())
    {
        BOOST_ASSERT(! key().empty());
        auto result =
            jv.raw_object().emplace(
                key(), kind::object);
        stack_.push_front(
            &result.first->second);
    }
    else if(jv.is_array())
    {
        BOOST_ASSERT(key().empty());
        jv.raw_array().emplace_back(kind::object);
        stack_.push_front(&jv.raw_array().back());
    }
    else
    {
        BOOST_ASSERT(jv.is_null());
        jv = kind::object;
    }
}

void
parser::
on_object_end(error_code&)
{
    BOOST_ASSERT(stack_.front()->is_object());
    stack_.pop_front();
}

void
parser::
on_array_begin(error_code& ec)
{
    if(stack_.size() >= max_depth_)
    {
        ec = error::too_deep;
        return;
    }
    auto& jv = *stack_.front();
    if(jv.is_object())
    {
        BOOST_ASSERT(! key().empty());
        auto result =
            jv.raw_object().emplace(
                key(), kind::array);
        stack_.push_front(
            &result.first->second);
    }
    else if(jv.is_array())
    {
        BOOST_ASSERT(key().empty());
        jv.raw_array().emplace_back(kind::array);
        stack_.push_front(&jv.raw_array().back());
    }
    else
    {
        BOOST_ASSERT(jv.is_null());
        jv = kind::array;
    }
}

void
parser::
on_array_end(error_code&)
{
    BOOST_ASSERT(stack_.front()->is_array());
    stack_.pop_front();
}

void
parser::
on_string_begin(error_code&)
{
    s_.clear();
}

void
parser::
on_string_piece(
    string_view s, error_code&)
{
    s_.append(s.data(), s.size());
}

void
parser::
on_string_end(error_code&)
{
    assign(s_);
    s_.clear();
}

void
parser::
on_number(number n, error_code&)
{
    if(n.is_integral())
    {
        assign(n.mant);
    }
    else
    {
        long exp = n.pos ?
            n.exp : -static_cast<long>(n.exp);
        auto mul = std::pow(10., exp);
        auto d = (n.neg ? -1. : 1.) * n.mant * mul;
        assign(d);
    }
}

void
parser::
on_bool(bool b, error_code&)
{
    assign(b);
}

void
parser::
on_null(error_code&)
{
    assign(null);
}

} // json
} // beast
} // boost

#endif
