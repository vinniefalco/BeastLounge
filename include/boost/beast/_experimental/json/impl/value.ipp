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
// special members
//

value::
~value()
{
    clear();
}

value::
value()
    : value(get_default_storage_ptr())
{
}

value::
value(value&& other)
{
    move(other.get_storage(), std::move(other));
}

value::
value(value const& other)
{
    copy(other.get_storage(), other);
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

value::
value(storage_ptr sp)
    : kind_(kind::null)
{
    ::new(&nat_.sp_)
        storage_ptr(std::move(sp));
}

//------------------------------------------------------------------------------

void
value::
set_kind(kind k) noexcept
{
    auto sp = release_storage();
    clear();
    switch(k)
    {
    case kind::object:
        // requires: noexcept construction
        ::new(&obj_) object(
            object::allocator_type(
                std::move(sp)));
        break;

    case kind::array:
        // requires: noexcept construction
        ::new(&arr_) array(
            array::allocator_type(
                std::move(sp)));
        break;

    case kind::string:
        // requires: noexcept construction
        ::new(&str_) string_type(
            string_type::allocator_type(
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

//------------------------------------------------------------------------------

// modifiers

value&
value::
operator[](key_param key)
{
    // implicit conversion to object from null
    if(is_null())
        set_kind(kind::object);
    else
        BOOST_ASSERT(is_object());
    // VFALCO unnecessary string conversion
    auto s = key.str.to_string();
    auto it = obj_.find(s);
    if(it == obj_.end())
        it = obj_.emplace(s, null);
    return it->second;
}

value const&
value::
operator[](key_param key) const
{
    BOOST_ASSERT(is_object());
    // VFALCO unnecessary string conversion
    auto s = key.str.to_string();
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
        return obj_.get_allocator().get_storage();

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
        return obj_.get_allocator().get_storage();

    case kind::array:
        return arr_.get_allocator().get_storage();

    case kind::string:
        return str_.get_allocator().get_storage();
    }
    return std::move(nat_.sp_);
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
        str_.~string_type();
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
                std::move(other.obj_), typename
                object::allocator_type(sp));
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
            ::new(&arr_) string_type(
                std::move(other.str_), typename
                string_type::allocator_type(sp));
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
        other.str_.~string_type();
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
        nat_.double_ = other.nat_.double_;
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
                other.obj_, typename
                object::allocator_type(sp));
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
            ::new(&arr_) string_type(
                other.str_, typename
                string_type::allocator_type(sp));
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
        nat_.double_ = other.nat_.double_;
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
    switch(jv.get_kind())
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
