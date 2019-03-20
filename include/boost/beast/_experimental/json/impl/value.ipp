//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_VALUE_IPP
#define BOOST_BEAST_JSON_IMPL_VALUE_IPP

#include <boost/beast/_experimental/json/value.hpp>
#include <new>
#include <utility>

namespace boost {
namespace beast {
namespace json {

//------------------------------------------------------------------------------
//
// Special members
//
//------------------------------------------------------------------------------

value::
~value()
{
    clear();
}

value::
value(value&& other)
    : value(
        std::move(other),
        other.get_storage())
{
}

value::
value(
    value&& other,
    storage_ptr store)
{
    move(std::move(store), std::move(other));
}

value::
value(value const& other)
    : value(other, other.get_storage())
{
}

value::
value(
    value const& other,
    storage_ptr store)
{
    copy(std::move(store), other);
}

value&
value::
operator=(value&& other)
{
    auto sp = release_storage();
    clear();
    move(std::move(sp), std::move(other));
    return *this;
}

value&
value::
operator=(value const& other)
{
    if(this != &other)
    {
        auto sp = release_storage();
        clear();
        copy(std::move(sp), other);
    }
    return *this;
}

//------------------------------------------------------------------------------

value::
value() noexcept
    : value(
        kind::null,
        get_default_storage_ptr())
{
}

value::
value(storage_ptr store) noexcept
    : value(
        kind::null,
        std::move(store))
{
}

value::
value(json::kind k) noexcept
    : value(
        k,
        get_default_storage_ptr())
{
}

value::
value(
    json::kind k,
    storage_ptr store) noexcept
{
    construct(k, std::move(store));
}

value::
value(object obj) noexcept
    : obj_(std::move(obj))
    , kind_(kind::object)
{
}

value::
value(
    object obj,
    storage_ptr store)
    : obj_(
        std::move(obj),
        std::move(store))
    , kind_(kind::object)
{
}

value::
value(array arr) noexcept
    : arr_(std::move(arr))
    , kind_(kind::array)
{
}

value::
value(
    array arr,
    storage_ptr store)
    : arr_(std::move(arr),
        array::allocator_type(
            std::move(store)))
    , kind_(kind::array)
{
}

value::
value(string str) noexcept
    : str_(std::move(str))
    , kind_(kind::string)
{
}

value::
value(
    string str,
    storage_ptr store)
    : str_(std::move(str),
        string::allocator_type(
            std::move(store)))
    , kind_(kind::string)
{
}

value::
value(number num)
    : value(num,
        get_default_storage_ptr())
{
}

value::
value(
    number num,
    storage_ptr store)
    : kind_(kind::number)
{
    ::new(&nat_.num_) number(num);
    ::new(&nat_.sp_) storage_ptr(
        std::move(store));
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init)
    : value(init,
        get_default_storage_ptr())
{
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init,
    storage_ptr store)
    : value(kind::object, std::move(store))
{
    for(auto& e : init)
        obj_.emplace(
            e.first,
            std::move(e.second),
            get_storage());
}

#if 0
value::
value(std::initializer_list<value> init)
    : value(init,
        get_default_storage_ptr())
{
}

value::
value(std::initializer_list<
    std::pair<string_view, value>> init,
    storage_ptr store)
    : value(kind::array, std::move(store))
{
    for(auto& e : init)
        arr_.emplace_back(
            std::move(e),
            get_storage());
}
#endif

value&
value::
operator=(object obj)
{
    object tmp(
        std::move(obj),
        get_storage());
    clear();
    ::new(&obj_) object(
        std::move(tmp));
    kind_ = kind::object;
    return *this;
}

value&
value::
operator=(array arr)
{
    array tmp(
        std::move(arr),
        array::allocator_type(
            get_storage()));
    clear();
    ::new(&arr_) array(
        std::move(tmp));
    kind_ = kind::array;
    return *this;
}

value&
value::
operator=(string str)
{
    string tmp(
        std::move(str),
        string::allocator_type(
            get_storage()));
    clear();
    ::new(&str_) string(
        std::move(tmp));
    kind_ = kind::string;
    return *this;
}

//------------------------------------------------------------------------------

void
value::
reset(json::kind k) noexcept
{
    auto sp = release_storage();
    clear();
    construct(k, std::move(sp));
}

//------------------------------------------------------------------------------

// modifiers

value&
value::
operator[](key_param key)
{
    // implicit conversion to object from null
    if(is_null())
        reset(kind::object);
    else
        BOOST_ASSERT(is_object());
    // VFALCO unnecessary string conversion
    auto s = key.str().to_string();
    auto it = obj_.find(s);
    if(it == obj_.end())
        it = obj_.emplace(s, null).first;
    return it->second;
}

value const&
value::
operator[](key_param key) const
{
    BOOST_ASSERT(is_object());
    // VFALCO unnecessary string conversion
    auto s = key.str().to_string();
    auto it = obj_.find(s);
    if(it == obj_.end())
        BOOST_THROW_EXCEPTION(system_error{
            error_code{error::key_not_found}});
    return it->second;
}

#if 0
value&
value::
operator[](std::size_t i) noexcept
{
    BOOST_ASSERT(is_array());
    BOOST_ASSERT(as_array().size() > i);
    return as_array()[i];
}

value const&
value::
operator[](std::size_t i) const noexcept
{
    BOOST_ASSERT(is_array());
    BOOST_ASSERT(as_array().size() > i);
    return as_array()[i];
}
#endif

//------------------------------------------------------------------------------

// private

storage_ptr
value::
get_storage() const noexcept
{
    switch(kind_)
    {
    case kind::object:
        return obj_.get_storage();

    case kind::array:
        return arr_.get_allocator().get_storage();

    case kind::string:
        return str_.get_allocator().get_storage();
    }
    return nat_.sp_;
}

storage_ptr
value::
release_storage() noexcept
{
    switch(kind_)
    {
    case kind::object:
        return obj_.get_storage();

    case kind::array:
        return arr_.get_allocator().get_storage();

    case kind::string:
        return str_.get_allocator().get_storage();
    }
    return std::move(nat_.sp_);
}

void
value::
construct(
    json::kind k,
    storage_ptr sp) noexcept
{
    switch(k)
    {
    case kind::object:
        // requires: noexcept construction
        ::new(&obj_) object(std::move(sp));
        break;

    case kind::array:
        // requires: noexcept construction
        ::new(&arr_) array(
            array::allocator_type(
                std::move(sp)));
        break;

    case kind::string:
        // requires: noexcept construction
        ::new(&str_) string(
            string::allocator_type(
                std::move(sp)));
        break;

    case kind::number:
    case kind::boolean:
    case kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = k;
}

// doesn't set kind_
void
value::
clear() noexcept
{
    switch(kind_)
    {
    case kind::object:
        obj_.~object();
        break;

    case kind::array:
        arr_.~array();
        break;

    case kind::string:
        str_.~string();
        break;

    case kind::number:
        nat_.num_.~number();

    case kind::boolean:
    case kind::null:
        nat_.sp_.~storage_ptr();
        break;
    }
}

// unchecked
void
value::
move(
    storage_ptr sp,
    value&& other)
{
    switch(other.kind_)
    {
    case kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object(
                std::move(other.obj_), sp);
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.obj_.~object();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case kind::array:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) array(
                std::move(other.arr_), typename
                array::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.arr_.~array();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case kind::string:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) string(
                std::move(other.str_), typename
                string::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        sp = other.get_storage();
        other.str_.~string();
        ::new(&other.nat_.sp_)
            storage_ptr(std::move(sp));
        break;

    case kind::number:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        ::new(&nat_.num_) number(
            other.nat_.num_);
        break;

    case kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.bool_ = other.nat_.bool_;
        break;

    case kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = other.kind_;
    other.kind_ = kind::null;
}

// unchecked
void
value::
copy(
    storage_ptr sp,
    value const& other)
{
    switch(other.kind_)
    {
    case kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object(
                other.obj_, sp);
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case kind::array:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) array(
                other.arr_, typename
                array::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case kind::string:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) string(
                other.str_, typename
                string::allocator_type(sp));
    #ifndef BOOST_NO_EXCEPTIONS
        } 
        catch(...)
        {
            kind_ = kind::null;
            ::new(&nat_.sp_)
                storage_ptr(std::move(sp));
            throw;
        }
    #endif
        break;

    case kind::number:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        ::new(&nat_.num_) number(
            other.nat_.num_);
        break;

    case kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.bool_ = other.nat_.bool_;
        break;

    case kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = other.kind_;
}

//------------------------------------------------------------------------------

// friends

std::ostream&
operator<<(std::ostream& os, value const& jv)
{
    switch(jv.kind())
    {
    case kind::object:
        os << '{';
        for(auto it = jv.as_object().begin(),
            last = jv.as_object().end();
            it != last;)
        {
            os << '\"' << it->first << "\":";
            os << it->second;
            if(++it != last)
                os << ',';
        }
        os << '}';
        break;
        
    case kind::array:
        os << '[';
        for(auto it = jv.as_array().begin(),
            last = jv.as_array().end();
            it != last;)
        {
            os << *it;
            if(++it != last)
                os << ',';
        }
        os << ']';
        break;
        
    case kind::string:
        os << '\"' << jv.as_string() << '\"';
        break;
        
    case kind::number:
        os << jv.as_number();
        break;
        
    case kind::boolean:
        if(jv.as_bool())
            os << "true";
        else
            os << "false";
        break;

    case kind::null:
        os << "null";
        break;
    }
    return os;
}

} // json
} // beast
} // boost

#endif
