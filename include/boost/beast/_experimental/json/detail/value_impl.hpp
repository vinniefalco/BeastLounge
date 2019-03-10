//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_DETAIL_VALUE_IMPL_HPP
#define BOOST_BEAST_JSON_DETAIL_VALUE_IMPL_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/kind.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <boost/beast/_experimental/json/types.hpp>

namespace boost {
namespace beast {
namespace json {
namespace detail {

class value_impl
{
    BOOST_BEAST_DECL
    void
    move(
        storage_ptr sp,
        value_impl&&);

    BOOST_BEAST_DECL
    void
    copy(
        storage_ptr sp,
        value_impl const&);

    BOOST_BEAST_DECL
    void
    clear() noexcept;

public:
    struct native_types
    {
        storage_ptr sp_;
        union
        {
            std::int64_t    int64_;
            std::uint64_t   uint64_;
            double          double_;
            bool            bool_;
        };
    };

    union
    {
        object_type     obj_;
        array_type      arr_;
        string_type     str_;
        native_types    nat_;
    };

    kind kind_;

    BOOST_BEAST_DECL
    ~value_impl();

    BOOST_BEAST_DECL
    value_impl();

    BOOST_BEAST_DECL
    value_impl(storage_ptr sp);

    BOOST_BEAST_DECL
    value_impl(value_impl&&);

    BOOST_BEAST_DECL
    value_impl(value_impl const&);

    BOOST_BEAST_DECL
    value_impl&
    operator=(value_impl&&);

    BOOST_BEAST_DECL
    value_impl&
    operator=(value_impl const&);

    BOOST_BEAST_DECL
    void
    set_kind(kind k) noexcept;    

    BOOST_BEAST_DECL
    storage_ptr const&
    get_storage() const noexcept;

    BOOST_BEAST_DECL
    storage_ptr
    release_storage() noexcept;
};

} // detail
} // json
} // beast
} // boost

#endif
