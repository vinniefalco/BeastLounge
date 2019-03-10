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

namespace detail {

// unchecked
void
value_impl::
move(
    storage_ptr sp,
    value_impl&& other)
{
    switch(other.kind_)
    {
    case kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object_type(
                std::move(other.obj_), typename
                object_type::allocator_type(
                    std::move(sp)));
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
        other.set_kind(kind::null);
        break;

    case kind::array:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) array_type(
                std::move(other.arr_), typename
                array_type::allocator_type(
                    std::move(sp)));
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
        other.set_kind(kind::null);
        break;

    case kind::string:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&arr_) string_type(
                std::move(other.str_), typename
                string_type::allocator_type(
                    std::move(sp)));
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
        other.set_kind(kind::null);
        break;

    case kind::signed64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.int64_ = other.nat_.int64_;
        other.kind_ = kind::null;
        break;

    case kind::unsigned64:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.uint64_ = other.nat_.uint64_;
        other.kind_ = kind::null;
        break;

    case kind::boolean:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        nat_.bool_ = other.nat_.bool_;
        other.kind_ = kind::null;
        break;

    case kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        other.kind_ = kind::null;
        break;
    }
    kind_ = other.kind_;
}

// unchecked
void
value_impl::
copy(
    storage_ptr sp,
    value_impl const& other)
{
    switch(other.kind_)
    {
    case kind::object:
    #ifndef BOOST_NO_EXCEPTIONS
        try
        {
    #endif
            ::new(&obj_) object_type(
                other.obj_, typename
                object_type::allocator_type(
                    std::move(sp)));
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
            ::new(&arr_) array_type(
                other.arr_, typename
                array_type::allocator_type(
                    std::move(sp)));
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
                string_type::allocator_type(
                    std::move(sp)));
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

// doesn't set kind_
void
value_impl::
clear() noexcept
{
    switch(kind_)
    {
    case kind::object:
        obj_.~object_type();
        break;

    case kind::array:
        arr_.~array_type();
        break;

    case kind::string:
        str_.~string_type();
        break;

    case kind::signed64:
    case kind::unsigned64:
    case kind::boolean:
    case kind::null:
        nat_.sp_.~storage_ptr();
        break;
    }
}

//------------------------------------------------------------------------------

value_impl::
~value_impl()
{
    clear();
}

value_impl::
value_impl()
    : value_impl(get_default_storage_ptr())
{
}

value_impl::
value_impl(storage_ptr sp)
    : kind_(kind::null)
{
    ::new(&nat_.sp_)
        storage_ptr(std::move(sp));
}

value_impl::
value_impl(value_impl&& other)
{
    move(other.get_storage(), std::move(other));
}

value_impl::
value_impl(value_impl const& other)
{
    copy(other.get_storage(), other);
}

value_impl&
value_impl::
operator=(value_impl&& other)
{
    auto sp = release_storage();
    clear();
    move(std::move(sp), std::move(other));
    return *this;
}

value_impl&
value_impl::
operator=(value_impl const& other)
{
    if(this != &other)
    {
        auto sp = release_storage();
        clear();
        copy(std::move(sp), other);
    }
    return *this;
}

void
value_impl::
set_kind(kind k) noexcept
{
    auto sp = release_storage();
    clear();
    switch(k)
    {
    case kind::object:
        // requires: noexcept construction
        ::new(&obj_) object_type(
            object_type::allocator_type(
                std::move(sp)));
        break;

    case kind::array:
        // requires: noexcept construction
        ::new(&arr_) array_type(
            array_type::allocator_type(
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
    case kind::boolean:
    case kind::null:
        ::new(&nat_.sp_)
            storage_ptr(std::move(sp));
        break;
    }
    kind_ = k;
}

storage_ptr const&
value_impl::
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

    case kind::signed64:
    case kind::unsigned64:
    case kind::boolean:
    case kind::null:
        break;
    }
    return nat_.sp_;
}

storage_ptr
value_impl::
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

    case kind::signed64:
    case kind::unsigned64:
    case kind::boolean:
    case kind::null:
        break;
    }
    return std::move(nat_.sp_);
}

} // detail

//------------------------------------------------------------------------------

// special

value::
value(storage_ptr sp)
    : impl_(std::move(sp))
{
}

// modifiers

value&
value::
operator[](key_param key)
{
    // VFALCO unnecessary string conversion
    auto s = key.str.to_string();
    auto it = impl_.obj_.find(s);
    if(it == impl_.obj_.end())
        it = impl_.obj_.emplace(s, null);
    return it->second;
}

#if 0
ref
value::
operator[](std::size_t i)
{
}
#endif


// observers

bool
value::
is_number() const noexcept
{
    switch(get_kind())
    {
    case kind::signed64:
    case kind::unsigned64:
    case kind::floating:
        return true;
    }
    return false;
}

bool
value::
is_primitive() const noexcept
{
    switch(get_kind())
    {
    case kind::object:
    case kind::array:
        return false;
    }
    return true;
}

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
