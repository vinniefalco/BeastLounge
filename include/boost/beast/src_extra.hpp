//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_SRC_EXTRA_HPP
#define BOOST_BEAST_SRC_EXTRA_HPP

/*

This file is meant to be included once, in a translation unit of
the program, with the macro BOOST_BEAST_SPLIT_COMPILATION defined.

*/

#define BOOST_BEAST_SOURCE

#include <boost/beast/core/detail/config.hpp>

#if defined(BOOST_BEAST_HEADER_ONLY)
# error Do not compile Beast library source with BOOST_BEAST_HEADER_ONLY defined
#endif

//#include <boost/beast/_experimental/json/detail/storage_base.ipp>
//#include <boost/beast/_experimental/json/detail/variant.ipp>
//#include <boost/beast/_experimental/json/impl/array_ref.ipp>
#include <boost/beast/_experimental/json/impl/basic_parser.ipp>
//#include <boost/beast/_experimental/json/impl/base.ipp>
#include <boost/beast/_experimental/json/impl/error.ipp>
//#include <boost/beast/_experimental/json/impl/init_list.ipp>
//#include <boost/beast/_experimental/json/impl/object_ref.ipp>
#include <boost/beast/_experimental/json/impl/parse_file.ipp>
#include <boost/beast/_experimental/json/impl/parser.ipp>
//#include <boost/beast/_experimental/json/impl/ptr.ipp>
//#include <boost/beast/_experimental/json/impl/ref.ipp>
#include <boost/beast/_experimental/json/impl/storage.ipp>
#include <boost/beast/_experimental/json/impl/value.ipp>

#endif
