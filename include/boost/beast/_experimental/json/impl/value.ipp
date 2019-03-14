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

value&
value::
operator=(object obj)
{
    auto sp = release_storage();
    clear();
    try
    {
        ::new(&obj_) object(
            std::move(obj),
            std::move(sp));
        kind_ = kind::object;
    }
    catch(...)
    {
        new(&nat_.sp_)
            storage_ptr(std::move(sp));
        kind_ = kind::null;
        throw;
    }
    return *this;
}

value&
value::
operator=(array arr)
{
    auto sp = release_storage();
    clear();
    try
    {
        ::new(&arr_) array(
            std::move(arr),
            array::allocator_type(
                std::move(sp)));
        kind_ = kind::array;
    }
    catch(...)
    {
        new(&nat_.sp_)
            storage_ptr(std::move(sp));
        kind_ = kind::null;
        throw;
    }
    return *this;
}

value&
value::
operator=(string str)
{
    auto sp = release_storage();
    clear();
    try
    {
        ::new(&str_) string(
            std::move(str),
            string::allocator_type(
                std::move(sp)));
        kind_ = kind::string;
    }
    catch(...)
    {
        new(&nat_.sp_)
            storage_ptr(std::move(sp));
        kind_ = kind::null;
        throw;
    }
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

value&
value::
operator[](std::size_t i) noexcept
{
    BOOST_ASSERT(is_array());
    BOOST_ASSERT(raw_array().size() > i);
    return raw_array()[i];
}

value const&
value::
operator[](std::size_t i) const noexcept
{
    BOOST_ASSERT(is_array());
    BOOST_ASSERT(raw_array().size() > i);
    return raw_array()[i];
}

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

    case kind::signed64:
    case kind::unsigned64:
    case kind::floating:
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

    case kind::signed64:
    case kind::unsigned64:
    case kind::floating:
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

    case kind::signed64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.int64_ = other.nat_.int64_;
        break;

    case kind::unsigned64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.uint64_ = other.nat_.uint64_;
        break;

    case kind::floating:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.float_ = other.nat_.float_;
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

    case kind::signed64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.int64_ = other.nat_.int64_;
        break;

    case kind::unsigned64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.uint64_ = other.nat_.uint64_;
        break;

    case kind::floating:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.float_ = other.nat_.float_;
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
        for(auto it = jv.raw_object().begin(),
            end = jv.raw_object().end();
            it != end;)
        {
            os << '\"' << it->first << "\":";
            os << it->second;
            if(++it != end)
                os << ',';
        }
        os << '}';
        break;
        
    case kind::array:
        os << '[';
        for(auto it = jv.raw_array().begin(),
            end = jv.raw_array().end();
            it != end;)
        {
            os << *it;
            if(++it != end)
                os << ',';
        }
        os << ']';
        break;
        
    case kind::string:
        os << '\"' << jv.raw_string() << '\"';
        break;
        
    case kind::signed64:
        os << jv.raw_signed();
        break;
        
    case kind::unsigned64:
        os << jv.raw_unsigned();
        break;

    case kind::floating:
        os << jv.raw_floating();
        break;
        
    case kind::boolean:
        if(jv.raw_bool())
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
