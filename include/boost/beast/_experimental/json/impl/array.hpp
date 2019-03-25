//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_ARRAY_HPP
#define BOOST_BEAST_JSON_IMPL_ARRAY_HPP

#include <boost/beast/_experimental/json/value.hpp>
#include <boost/core/exchange.hpp>
#include <boost/static_assert.hpp>
#include <algorithm>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

//------------------------------------------------------------------------------

class array::table
{
    union
    {
        std::size_t capacity_;
        value unused_; // for alignment
    };

    BOOST_STATIC_ASSERT(
        sizeof(value) >= sizeof(std::size_t));

    BOOST_BEAST_DECL;
    ~table();

public:
    size_type size = 0;

    explicit
    table(size_type capacity)
        : capacity_(capacity)
    {
    }

    size_type
    capacity() const noexcept
    {
        return capacity_;
    }

    value_type*
    begin() noexcept
    {
        return reinterpret_cast<
            value_type*>(this + 1);
    }

    value_type*
    end() noexcept
    {
        return begin() + size;
    }

    BOOST_BEAST_DECL;
    static
    table*
    create(
        size_type capacity,
        storage_ptr const& sp);

    BOOST_BEAST_DECL;
    static
    void
    destroy(
        table* tab,
        storage_ptr const& sp);
};

//------------------------------------------------------------------------------

struct array::cleanup_assign
{
    array& self;
    table* tab;
    bool ok = false;

    BOOST_BEAST_DECL;
    explicit
    cleanup_assign(
        array& self);

    BOOST_BEAST_DECL;
    ~cleanup_assign();
};

//------------------------------------------------------------------------------

struct array::cleanup_insert
{
    array& self;
    size_type pos;
    size_type n;
    size_type valid = 0;
    bool ok = false;

    BOOST_BEAST_DECL;
    cleanup_insert(
        size_type pos_,
        size_type n_,
        array& self_);

    BOOST_BEAST_DECL;
    ~cleanup_insert();
};

//------------------------------------------------------------------------------

template<class InputIt>
array::
array(
    InputIt first, InputIt last)
    : array(
        first, last,
        default_storage())
{
}

template<class InputIt>
array::
array(
    InputIt first, InputIt last,
    storage_ptr store)
    : array(
        first, last,
        std::move(store),
        iter_cat<InputIt>{})
{
}

template<class InputIt>
auto
array::
insert(
    const_iterator before,
    InputIt first, InputIt last) ->
        iterator
{
    return insert(before, first, last,
        iter_cat<InputIt>{});
}

template<class... Args>
auto
array::
emplace(
    const_iterator before,
    Args&&... args) ->
        iterator
{
    return emplace_impl(before,
        value_type(std::forward<Args>(args)...),
        sp_);
}

template<class... Args>
auto
array::
emplace_back(Args&&... args) ->
    reference
{
    return emplace_back_impl(
        value_type(std::forward<Args>(args)...),
        sp_);
}

//------------------------------------------------------------------------------

template<class InputIt>
array::
array(
    InputIt first, InputIt last,
    storage_ptr store,
    std::input_iterator_tag)
    : sp_(std::move(store))
{
    while(first != last)
        emplace_back_impl(
            value_type(*first++, sp_));
}

template<class InputIt>
array::
array(
    InputIt first, InputIt last,
    storage_ptr store,
    std::forward_iterator_tag)
    : sp_(std::move(store))
{
    reserve(std::distance(first, last));
    while(first != last)
        emplace_back_impl(
            value_type(*first++, sp_));
}
template<class InputIt>
auto
array::
insert(
    const_iterator before,
    InputIt first, InputIt last,
    std::input_iterator_tag) ->
        iterator
{
    auto pos = before - begin();
    while(first != last)
        before = insert(before, *first++) + 1;
    return begin() + pos;
}

template<class InputIt>
auto
array::
insert(
    const_iterator before,
    InputIt first, InputIt last,
    std::forward_iterator_tag) ->
        iterator
{
    auto count = std::distance(first, last);
    auto pos = before - begin();
    cleanup_insert c(pos, count, *this);
    while(count--)
    {
        ::new(&begin()[pos++]) value_type(
            *first++, sp_);
        ++c.valid_;
    }
    c.ok = true;
    return begin() + c.pos;
}

template<class... Args>
auto
array::
emplace_impl(
    const_iterator before,
    Args&&... args) ->
        iterator
{
    cleanup_insert c(
        before - begin(), 1, *this);
    ::new(&tab_->begin()[c.pos]) value_type(
        std::forward<Args>(args)...);
    c.ok = true;
    return begin() + c.pos;
}

template<class... Args>
auto
array::
emplace_back_impl(
    Args&&... args) ->
    reference
{
    reserve(size() + 1);
    ::new(&tab_->begin()[size()]) value_type(
        std::forward<Args>(args)...);
    ++tab_->size;
    return back();
}


} // json
} // beast
} // boost

#endif
