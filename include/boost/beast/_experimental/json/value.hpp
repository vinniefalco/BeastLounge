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
#include <boost/beast/_experimental/json/array.hpp>
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/beast/_experimental/json/key_param.hpp>
#include <boost/beast/_experimental/json/kind.hpp>
#include <boost/beast/_experimental/json/number.hpp>
#include <boost/beast/_experimental/json/object.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <boost/beast/_experimental/json/string.hpp>
#include <boost/beast/_experimental/json/detail/has_assign_value.hpp>
#include <boost/beast/_experimental/json/detail/is_specialized.hpp>
#include <boost/type_traits/make_void.hpp>
#include <cstdlib>
#include <initializer_list>
#include <iosfwd>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

class value;

/** Customization point for assigning to and from class types.
*/
template<class T>
struct value_exchange final
#ifndef BOOST_BEAST_DOXYGEN
    : detail::primary_template
#endif
{
    static
    void
    assign(value& v, T const& t)
    {
        detail::call_assign(v, t);
    }

    //[[nodiscard]]
    static
    void
    assign(T& t, value const& v, error_code& ec)
    {
        detail::call_assign(t, v, ec);
    }
};

/** Trait to determine if a type can be assigned to a json value.
*/
template<class T>
using can_value_to =
#ifdef BOOST_BEAST_DOXYGEN
    __see_below__;
#else
    std::integral_constant<bool,
        detail::is_specialized<value_exchange<
            detail::remove_cr<T>>>::value ||
        detail::has_adl_value_to<
            detail::remove_cr<T>>::value ||
        detail::has_mf_value_to<
            detail::remove_cr<T>>::value>;
#endif

/** Trait to determine if a json value can be assigned to a type.
*/
template<class T>
using can_value_from =
#ifdef BOOST_BEAST_DOXYGEN
    __see_below__;
#else
    std::integral_constant<bool,
        detail::is_specialized<value_exchange<
            detail::remove_cr<T>>>::value ||
        detail::has_adl_value_from<
            detail::remove_cr<T>>::value ||
        detail::has_mf_value_from<
            detail::remove_cr<T>>::value>;
#endif

//------------------------------------------------------------------------------

/** A JSON value.
*/
class value
{
    friend class value_test;

    struct native
    {
        union
        {
            number  num_;
            bool    bool_;
        };
        storage_ptr sp_;
    };

    union
    {
        object    obj_;
        array     arr_;
        string    str_;
        native    nat_;
    };

    json::kind kind_;

public:

    //--------------------------------------------------------------------------
    //
    // Special members
    //
    //--------------------------------------------------------------------------

    /// Destroy a value and all of its contents
    BOOST_BEAST_DECL
    ~value();

    /// Move constructor
    BOOST_BEAST_DECL
    value(value&& other);

    /// Move construct a value, using the specified storage
    BOOST_BEAST_DECL
    value(
        value&& other,
        storage_ptr store);

    /// Construct a copy of a value
    BOOST_BEAST_DECL
    value(value const& other);

    /// Construct a copy of a value using the specified storage
    BOOST_BEAST_DECL
    value(
        value const& other,
        storage_ptr store);

    /// Move-assign a value
    BOOST_BEAST_DECL
    value& operator=(value&& other);

    /// Assign a copy of a value
    BOOST_BEAST_DECL
    value& operator=(value const& other);

    //--------------------------------------------------------------------------
    //
    // Construction and Assignment
    //
    //--------------------------------------------------------------------------

    /** Construct a null value using the default storage.
    */
    BOOST_BEAST_DECL
    value() noexcept;

    /** Construct a null value using the specified storage.

        The value and all of its contents will use the
        specified storage object.
    */
    BOOST_BEAST_DECL
    explicit
    value(storage_ptr store) noexcept;

    /** Construct a value using the default storage

        The value and all of its contents will use the
        specified storage object.
    */
    BOOST_BEAST_DECL
    value(json::kind k) noexcept;

    /** Construct a value using the specified storage.

        The value and all of its contents will use the specified
        storage object.

        @param k The kind of JSON value.

        @param store The storage to use.
    */
    BOOST_BEAST_DECL
    value(
        json::kind k,
        storage_ptr store) noexcept;

    /** Construct a value from an object.
    */
    BOOST_BEAST_DECL
    value(object obj) noexcept;

    /** Construct a value from an object using the specified storage
    */
    BOOST_BEAST_DECL
    value(object obj, storage_ptr store);

    /** Construct a value from an array.
    */
    BOOST_BEAST_DECL
    value(array arr) noexcept;

    /** Construct a value from an array using the specified storage
    */
    BOOST_BEAST_DECL
    value(array arr, storage_ptr store);

    /** Construct a value from a string.
    */
    BOOST_BEAST_DECL
    value(string str) noexcept;

    /** Construct a value from a string using the specified storage
    */
    BOOST_BEAST_DECL
    value(string str, storage_ptr store);

    /** Construct a value from a nujmber
    */
    BOOST_BEAST_DECL
    value(number num);

    /** Construct a value from a nujmber using the specified storage
    */
    BOOST_BEAST_DECL
    value(number num, storage_ptr store);

    /** Construct an object from an initializer list.
    */
    BOOST_BEAST_DECL
    value(std::initializer_list<
        std::pair<string_view, value>> init);

    /** Construct an object from an initializer list using the specified storage
    */
    BOOST_BEAST_DECL
    value(std::initializer_list<
        std::pair<string_view, value>> init,
        storage_ptr store);
#if 0
    /** Construct an array from an initializer list.
    */
    BOOST_BEAST_DECL
    value(std::initializer_list<value> init);

    /** Construct an array from an initializer list using the specified storage.
    */
    BOOST_BEAST_DECL
    value(std::initializer_list<value> init,
        storage_ptr store);
#endif

    /** Assign a value from an object
    */
    BOOST_BEAST_DECL
    value&
    operator=(object obj);

    /** Assign a value from an array
    */
    BOOST_BEAST_DECL
    value&
    operator=(array arr);

    /** Assign a value from a string
    */
    BOOST_BEAST_DECL
    value&
    operator=(string str);

    //--------------------------------------------------------------------------
    //
    // Modifiers
    //
    //--------------------------------------------------------------------------

    /** Reset the json to the specified type.

        This changes the value to hold a value of the
        specified type. Any previous contents are cleared.

        @param k The kind to set. If the new kind is an
        object, array, or string the resulting value will be
        empty. Otherwise, the value will be in an undefined,
        valid state.
    */
    BOOST_BEAST_DECL
    void
    reset(json::kind k = json::kind::null) noexcept;

    /** Reset the json to the specified type.

        This changes the value to hold a value of the
        specified type. Any previous contents are cleared.

        @param k The kind to set. If the new kind is an
        object, array, or string the resulting value will be
        empty. Otherwise, the value will be in an undefined,
        valid state.
    */
    BOOST_BEAST_DECL
    value&
    operator=(json::kind k) noexcept
    {
        reset(k);
        return *this;
    }

    /** Set the value to an empty object, and return it.

        This calls `reset(json::kind::object)` and returns
        `as_object()`. The previous contents of the value
        are destroyed.
    */
    object&
    emplace_object() noexcept
    {
        reset(json::kind::object);
        return as_object();
    }

    /** Set the value to an empty array, and return it.

        This calls `reset(json::kind::array)` and returns
        `as_array()`. The previous contents of the value
        are destroyed.
    */
    array&
    emplace_array() noexcept
    {
        reset(json::kind::array);
        return as_array();
    }

    /** Set the value to an empty string, and return it.

        This calls `reset(json::kind::string)` and returns
        `as_string()`. The previous contents of the value
        are destroyed.
    */
    string&
    emplace_string() noexcept
    {
        reset(json::kind::string);
        return as_string();
    }

    /** Set the value to an uninitialized number, and return it.

        This calls `reset(json::kind::number)` and returns
        `as_number()`. The previous contents of the value
        are destroyed.
    */
    number&
    emplace_number() noexcept
    {
        reset(json::kind::number);
        return as_number();
    }

    /** Set the value to an uninitialized boolean, and return it.

        This calls `reset(json::kind::boolean)` and returns
        `as_bool()`. The previous contents of the value
        are destroyed.
    */
    bool&
    emplace_bool() noexcept
    {
        reset(json::kind::boolean);
        return as_bool();
    }

    //--------------------------------------------------------------------------
    //
    // Exchange
    //
    //--------------------------------------------------------------------------

    /// Construct a value from another type
    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            can_value_from<T>::value>::type
    #endif
    >
    value(T const& t)
        : value(t, get_default_storage_ptr())
    {
    }

    /// Construct a value from another type using the specified storage
    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            can_value_from<T>::value>::type
    #endif
    >
    value(T const& t, storage_ptr store)
        : value(std::move(store))
    {
        value_exchange<
            detail::remove_cr<T>
                >::assign(*this, t);
    }

    /// Assign a value from another type
    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            can_value_from<T>::value>::type
    #endif
    >
    value&
    operator=(T const& t)
    {
        value_exchange<
            detail::remove_cr<T>
                >::assign(*this, t);
        return *this;
    }

    /** Try to assign a value to another type

        @throws system error Thrown upon failure
    */
    template<class T>
    void
    assign(T& t) const
    {
        error_code ec;
        this->assign(t, ec);
        if(ec)
            BOOST_THROW_EXCEPTION(
                system_error{ec});
    }

    /** Try to assign a value to another type

        @param ec Set to the error, if any occurred.
    */
    template<class T>
    void
    assign(T& t, error_code& ec) const
    {
        // If this assert goes off, it means that there no known
        // way to assign from a `value` to a user defined type `T`.
        // There are three ways to fix this:
        //
        // 1. Add the member function `T::assign(value const&, error_code&)`,
        //
        // 2. Add the free function `assign(T&, value const&, error_code&)`
        //    in the same namespace as T, or
        //
        // 3. Specialize `json::value_exchange` for `T`, and provide the static
        //    member `value_exchange<T>::assign(T&, value const&, error_code&)`.

        static_assert(
            can_value_to<T>::value,
            "Destination type is unknown");
            value_exchange<
                detail::remove_cr<T>
                    >::assign(t, *this, ec);
    }
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    value&
    operator[](key_param key);

    BOOST_BEAST_DECL
    value const&
    operator[](key_param key) const;

#if 0
    BOOST_BEAST_DECL
    value&
    value::
    operator[](std::size_t i) noexcept;

    BOOST_BEAST_DECL
    value const&
    value::
    operator[](std::size_t i) const noexcept;
#endif

    //--------------------------------------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------------------------------------

    /// Returns the kind of this JSON value
    json::kind
    kind() const noexcept
    {
        return kind_;
    }

    /// Returns `true` if this is an object
    bool
    is_object() const noexcept
    {
        return kind_ == json::kind::object;
    }

    /// Returns `true` if this is an array
    bool
    is_array() const noexcept
    {
        return kind_ == json::kind::array;
    }

    /// Returns `true` if this is a string
    bool
    is_string() const noexcept
    {
        return kind_ == json::kind::string;
    }

    /// Returns `true` if this is a number
    bool
    is_number() const noexcept
    {
        return kind_ == json::kind::number;
    }

    bool
    is_bool() const noexcept
    {
        return kind_ == json::kind::boolean;
    }

    bool
    is_null() const noexcept
    {
        return kind_ == json::kind::null;
    }

    //---

    /// Returns `true` if this is not an array or object
    bool
    is_primitive() const noexcept
    {
        switch(kind_)
        {
        case json::kind::object:
        case json::kind::array:
            return false;
        default:
            return true;
        }

    }

    /// Returns `true` if this is an array or object
    bool
    is_structured() const noexcept
    {
        return ! is_primitive();
    }

    /// Returns `true` if this is a number representable as a `std::int64_t`
    bool
    is_int64() const noexcept
    {
        return
            kind_ == json::kind::number &&
            nat_.num_.is_int64();
    }

    /// Returns `true` if this is a number representable as a `std::uint64_t`
    bool
    is_uint64() const noexcept
    {
        return
            kind_ == json::kind::number &&
            nat_.num_.is_uint64();
    }

    /** Returns `true` if this is a number representable as a `double`

        The return value will always be the same as the
        value returned from @ref is_number.
    */
    bool
    is_double() const noexcept
    {
        return kind_ == json::kind::number;
    }

    //--------------------------------------------------------------------------
    //
    // Accessors
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    storage_ptr
    get_storage() const noexcept;

    object&
    as_object() noexcept
    {
        BOOST_ASSERT(is_object());
        return obj_;
    }

    object const&
    as_object() const noexcept
    {
        BOOST_ASSERT(is_object());
        return obj_;
    }

    array&
    as_array() noexcept
    {
        BOOST_ASSERT(is_array());
        return arr_;
    }

    array const&
    as_array() const noexcept
    {
        BOOST_ASSERT(is_array());
        return arr_;
    }

    string&
    as_string() noexcept
    {
        BOOST_ASSERT(is_string());
        return str_;
    }

    string const&
    as_string() const noexcept
    {
        BOOST_ASSERT(is_string());
        return str_;
    }

    number&
    as_number() noexcept
    {
        BOOST_ASSERT(is_number());
        return nat_.num_;
    }

    number const&
    as_number() const noexcept
    {
        BOOST_ASSERT(is_number());
        return nat_.num_;
    }

    std::int64_t
    get_int64() const noexcept
    {
        BOOST_ASSERT(is_int64());
        return nat_.num_.get_int64();
    }

    std::uint64_t
    get_uint64() const noexcept
    {
        BOOST_ASSERT(is_uint64());
        return nat_.num_.get_uint64();
    }

    long double
    get_double() const noexcept
    {
        BOOST_ASSERT(is_double());
        return nat_.num_.get_double();
    }

    bool&
    as_bool() noexcept
    {
        BOOST_ASSERT(is_bool());
        return nat_.bool_;
    }

    bool const&
    as_bool() const noexcept
    {
        BOOST_ASSERT(is_bool());
        return nat_.bool_;
    }

    //--------------------------------------------------------------------------

private:
    BOOST_BEAST_DECL
    storage_ptr
    release_storage() noexcept;

    BOOST_BEAST_DECL
    void
    construct(
        json::kind, storage_ptr) noexcept;

    BOOST_BEAST_DECL
    void
    clear() noexcept;

    BOOST_BEAST_DECL
    void
    move(storage_ptr, value&&);

    BOOST_BEAST_DECL
    void
    copy(storage_ptr, value const&);

    BOOST_BEAST_DECL
    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        value const& jv);
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/array.hpp>
#include <boost/beast/_experimental/json/impl/object.hpp>
#include <boost/beast/_experimental/json/impl/value.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/array.ipp>
#include <boost/beast/_experimental/json/impl/object.ipp>
#include <boost/beast/_experimental/json/impl/value.ipp>
#endif

#endif
