//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_VALUE_HPP
#define BOOST_BEAST_JSON_VALUE_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/exchange.hpp>
#include <boost/beast/_experimental/json/key_param.hpp>
#include <boost/beast/_experimental/json/kind.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <boost/beast/_experimental/json/detail/value_impl.hpp>
#include <cstdlib>
#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

namespace boost {
namespace beast {
namespace json {

#ifndef BOOST_BEAST_DOXYGEN
namespace detail {
class access;
} // detail
#endif

/** A JSON value.
*/
class value
{
    detail::value_impl impl_;

    friend class detail::access;

public:
    /** Default Constructor

        A default-constructed value will be a JSON null.
    */
    value() = default;

    /** Construct a value associated with a storage instance.

        The value and all of its contents will use the specified
        storage object.
    */
    BOOST_BEAST_DECL
    explicit
    value(storage_ptr store);

    // modifiers

    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            is_exchange_type<T>::value>::type
    #endif
    >
    value(T const& t);

    /*
    BOOST_BEAST_DECL
    value(init_list list);
    */

    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            is_exchange_type<T>::value>::type
    #endif
    >
    value&
    operator=(T const& t);

    /*
    BOOST_BEAST_DECL
    value&
    operator=(init_list list);
    */

    BOOST_BEAST_DECL
    value&
    operator[](key_param key);

    BOOST_BEAST_DECL
    value&
    value::
    operator[](std::size_t i);

    // observers

    BOOST_BEAST_DECL
    kind
    get_kind() const noexcept
    {
        return impl_.kind_;
    }

    BOOST_BEAST_DECL
    bool
    is_number() const noexcept;

    BOOST_BEAST_DECL
    bool
    is_primitive() const noexcept;

    bool
    is_structured() const noexcept
    {
        return ! is_primitive();
    }

    bool
    is_object() const noexcept
    {
        return get_kind() == kind::object;
    }

    bool
    is_array() const noexcept
    {
        return get_kind() == kind::array;
    }

    bool
    is_string() const noexcept
    {
        return get_kind() == kind::string;
    }

    bool
    is_signed64() const noexcept
    {
        return get_kind() == kind::signed64;
    }

    bool
    is_unsigned64() const noexcept
    {
        return get_kind() == kind::unsigned64;
    }

    bool
    is_floating() const noexcept
    {
        return get_kind() == kind::floating;
    }

    bool
    is_boolean() const noexcept
    {
        return get_kind() == kind::boolean;
    }

    bool
    is_null() const noexcept
    {
        return get_kind() == kind::null;
    }

    // raw access

    raw_object_type&
    raw_object() noexcept
    {
        return impl_.obj_;
    }

    raw_object_type const&
    raw_object() const noexcept
    {
        return impl_.obj_;
    }

    raw_array_type&
    raw_array() noexcept
    {
        return impl_.arr_;
    }

    raw_array_type const&
    raw_array() const noexcept
    {
        return impl_.arr_;
    }

    raw_string_type&
    raw_string() noexcept
    {
        return impl_.str_;
    }

    raw_string_type const&
    raw_string() const noexcept
    {
        return impl_.str_;
    }

    raw_signed_type&
    raw_signed() noexcept
    {
        return impl_.nat_.int64_;
    }

    raw_signed_type const&
    raw_signed() const noexcept
    {
        return impl_.nat_.int64_;
    }

    raw_unsigned_type&
    raw_unsigned() noexcept
    {
        return impl_.nat_.uint64_;
    }

    raw_unsigned_type const&
    raw_unsigned() const noexcept
    {
        return impl_.nat_.uint64_;
    }

    raw_floating_type&
    raw_floating() noexcept
    {
        return impl_.nat_.double_;
    }

    raw_floating_type const&
    raw_floating() const noexcept
    {
        return impl_.nat_.double_;
    }

    raw_bool_type&
    raw_bool() noexcept
    {
        return impl_.nat_.bool_;
    }

    raw_bool_type const&
    raw_bool() const noexcept
    {
        return impl_.nat_.bool_;
    }

private:
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        value const& jv);
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/exchange.hpp>
#include <boost/beast/_experimental/json/impl/value.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/value.ipp>
#endif

#endif
