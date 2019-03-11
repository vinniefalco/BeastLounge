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
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/beast/_experimental/json/key_param.hpp>
#include <boost/beast/_experimental/json/kind.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <boost/beast/_experimental/json/types.hpp>
#include <boost/beast/_experimental/json/detail/is_specialized.hpp>
#include <boost/type_traits/make_void.hpp>
#include <cstdlib>
#include <iosfwd>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

#ifndef BOOST_BEAST_DOXYGEN

class value;

namespace detail {

// Determine if `assign(value&,T)` is found via ADL
template<class T, class = void>
struct has_adl_value_from : std::false_type
{
};

template<class T>
struct has_adl_value_from<T,
    boost::void_t<decltype(assign(
        std::declval<json::value&>(),
        std::declval<T>()))>>
    : std::true_type
{
};

// Determine if `error_code assign(T&,value)` is found via ADL
template<class T, class = void>
struct has_adl_value_to : std::false_type
{
};

template<class T>
struct has_adl_value_to<T,
    boost::void_t<decltype(assign(
        std::declval<T&>(),
        std::declval<json::value const&>(),
        std::declval<error_code&>()))>>
    : std::true_type
{
};

// Determine if `t.assign(value&)` exists
template<class T, class = void>
struct has_mf_value_from : std::false_type
{
};

template<class T>
struct has_mf_value_from<T,
    boost::void_t<decltype(
        std::declval<T const&>().assign(
            std::declval<json::value&>()))>>
    : std::true_type
{
};

// Determine if `t.assign(value const&,error_code&)` exists
template<class T, class = void>
struct has_mf_value_to : std::false_type
{
};

template<class T>
struct has_mf_value_to<T,
    boost::void_t<decltype(
    std::declval<T&>().assign(
        std::declval<json::value const&>(),
        std::declval<error_code&>()))>>
    : std::true_type
{
};

template<class T>
void
call_assign(value& v, T const& t, std::true_type)
{
    t.assign(v);
}

template<class T>
void
call_assign(value& v, T const& t, std::false_type)
{
    assign(v, t);
}

template<class T>
void
call_assign(value& v, T const& t)
{
    call_assign(v, t, has_mf_value_from<T>{});
}

template<class T>
void
call_assign(T& t, value const& v, error_code& ec, std::true_type)
{
    t.assign(v, ec);
}

template<class T>
void
call_assign(T& t, value const& v, error_code& ec, std::false_type)
{
    assign(t, v, ec);
}

template<class T>
void
call_assign(T& t, value const& v, error_code& ec)
{
    call_assign(t, v, ec, has_mf_value_to<T>{});
}

} // detail
#endif

//------------------------------------------------------------------------------

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

public:

    //
    // special members
    //

    /// Destroy a value and all of its contents
    BOOST_BEAST_DECL
    ~value();

    /** Default constructor

        A default-constructed value will be a JSON null.
    */
    BOOST_BEAST_DECL
    value();

    /// Move constructor
    BOOST_BEAST_DECL
    value(value&& other);

    /// Construct a copy of a value
    BOOST_BEAST_DECL
    value(value const& other);

    /// Move-assign a value
    BOOST_BEAST_DECL
    value& operator=(value&& other);

    /// Assign a copy of a value
    BOOST_BEAST_DECL
    value& operator=(value const& other);

    /** Construct a value associated with a storage instance.

        The value and all of its contents will use the specified
        storage object.
    */
    BOOST_BEAST_DECL
    explicit
    value(storage_ptr store);

    /// Construct a value from T
    template<
        class T
    #ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            can_value_from<T>::value>::type
    #endif
    >
    value(T const& t)
        : value()
    {
        value_exchange<
            detail::remove_cr<T>
                >::assign(*this, t);
    }

    /// Assign a value from T
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

    //--------------------------------------------------------------------------

    /** Set the json type of this value.

        This sets the kind of json this value holds.

        @param k The kind to set. If this is a structured
        type, the resulting container will be empty.
    */
    BOOST_BEAST_DECL
    void
    set_kind(kind k) noexcept;

    BOOST_BEAST_DECL
    value&
    operator[](key_param key);

    BOOST_BEAST_DECL
    value const&
    operator[](key_param key) const;

    BOOST_BEAST_DECL
    value&
    value::
    operator[](std::size_t i) noexcept;

    BOOST_BEAST_DECL
    value const&
    value::
    operator[](std::size_t i) const noexcept;

    /** Assign this json value to another type.

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

    /** Assign this json value to another type.

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

    //
    // query
    //

    kind
    get_kind() const noexcept
    {
        return kind_;
    }

    bool
    is_primitive() const noexcept
    {
        switch(kind_)
        {
        case kind::object:
        case kind::array:
            return false;
        }
        return true;
    }

    bool
    is_structured() const noexcept
    {
        return ! is_primitive();
    }

    bool
    is_number() const noexcept
    {
        switch(kind_)
        {
        case kind::signed64:
        case kind::unsigned64:
        case kind::floating:
            return true;
        }
        return false;
    }

    bool
    is_integer() const noexcept
    {
        switch(kind_)
        {
        case kind::signed64:
        case kind::unsigned64:
            return true;
        }
        return false;
    }

    bool
    is_object() const noexcept
    {
        return kind_ == kind::object;
    }

    bool
    is_array() const noexcept
    {
        return kind_ == kind::array;
    }

    bool
    is_string() const noexcept
    {
        return kind_ == kind::string;
    }

    bool
    is_signed64() const noexcept
    {
        return kind_ == kind::signed64;
    }

    bool
    is_unsigned64() const noexcept
    {
        return kind_ == kind::unsigned64;
    }

    bool
    is_floating() const noexcept
    {
        return kind_ == kind::floating;
    }

    bool
    is_boolean() const noexcept
    {
        return kind_ == kind::boolean;
    }

    bool
    is_null() const noexcept
    {
        return kind_ == kind::null;
    }

    // raw

    raw_object_type&
    raw_object() noexcept
    {
        return obj_;
    }

    raw_object_type const&
    raw_object() const noexcept
    {
        return obj_;
    }

    raw_array_type&
    raw_array() noexcept
    {
        return arr_;
    }

    raw_array_type const&
    raw_array() const noexcept
    {
        return arr_;
    }

    raw_string_type&
    raw_string() noexcept
    {
        return str_;
    }

    raw_string_type const&
    raw_string() const noexcept
    {
        return str_;
    }

    raw_signed_type&
    raw_signed() noexcept
    {
        return nat_.int64_;
    }

    raw_signed_type const&
    raw_signed() const noexcept
    {
        return nat_.int64_;
    }

    raw_unsigned_type&
    raw_unsigned() noexcept
    {
        return nat_.uint64_;
    }

    raw_unsigned_type const&
    raw_unsigned() const noexcept
    {
        return nat_.uint64_;
    }

    raw_floating_type&
    raw_floating() noexcept
    {
        return nat_.double_;
    }

    raw_floating_type const&
    raw_floating() const noexcept
    {
        return nat_.double_;
    }

    raw_bool_type&
    raw_bool() noexcept
    {
        return nat_.bool_;
    }

    raw_bool_type const&
    raw_bool() const noexcept
    {
        return nat_.bool_;
    }

private:
    BOOST_BEAST_DECL
    storage_ptr
    get_storage() const noexcept;

    BOOST_BEAST_DECL
    storage_ptr
    release_storage() noexcept;

    BOOST_BEAST_DECL
    void
    clear() noexcept;

    BOOST_BEAST_DECL
    void
    move(storage_ptr, value&&);

    BOOST_BEAST_DECL
    void
    copy(storage_ptr, value const&);

    friend
    std::ostream&
    operator<<(
        std::ostream& os,
        value const& jv);
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/value.hpp>
#ifdef BOOST_BEAST_HEADER_ONLY
#include <boost/beast/_experimental/json/impl/value.ipp>
#endif

#endif
