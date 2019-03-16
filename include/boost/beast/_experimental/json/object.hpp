//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_OBJECT_HPP
#define BOOST_BEAST_JSON_OBJECT_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/core/string.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <utility>

namespace boost {
namespace beast {
namespace json {

class value;

//------------------------------------------------------------------------------

class object
{
    struct list_hook;
    struct element;
    class table;

    storage_ptr sp_;
    table* tab_ = nullptr;
    float mf_ = 1.0;

public:
    using key_type = beast::string_view;

    using mapped_type = value;

    using value_type =
        std::pair<string_view, value>;

    using size_type = std::size_t;

    using difference_type = std::ptrdiff_t;

    class hasher;

    class key_equal;

    using reference =
        std::pair<string_view, value&>;

    using const_reference =
        std::pair<string_view, value const&>;

    class pointer;
    class const_pointer;
    class iterator;
    class const_iterator;
    class local_iterator;
    class const_local_iterator;
    class node_type;
    struct insert_return_type;

    //--------------------------------------------------------------------------
    //
    // Special Members
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    ~object();

    BOOST_BEAST_DECL
    object();

    BOOST_BEAST_DECL
    object(
        size_type bucket_count);

    BOOST_BEAST_DECL
    object(
        storage_ptr store);

    BOOST_BEAST_DECL
    object(
        size_type bucket_count,
        storage_ptr store);

    template<class InputIt>
    BOOST_BEAST_DECL
    object(
        InputIt first,
        InputIt last);

    template<class InputIt>
    BOOST_BEAST_DECL
    object(
        InputIt first,
        InputIt last,
        size_type bucket_count);

    template<class InputIt>
    BOOST_BEAST_DECL
    object(
        InputIt first,
        InputIt last,
        storage_ptr store);

    template<class InputIt>
    BOOST_BEAST_DECL
    object(
        InputIt first,
        InputIt last,
        size_type bucket_count,
        storage_ptr store);

    BOOST_BEAST_DECL
    object(object&& other) noexcept;

    BOOST_BEAST_DECL
    object(
        object&& other,
        storage_ptr store) noexcept;

    BOOST_BEAST_DECL
    object(
        object const& other);

    BOOST_BEAST_DECL
    object(
        object const& other,
        storage_ptr store);

    BOOST_BEAST_DECL
    object(
        std::initializer_list<value_type> init);

    BOOST_BEAST_DECL
    object(
        std::initializer_list<value_type> init,
        size_type bucket_count);

    BOOST_BEAST_DECL
    object(
        std::initializer_list<value_type> init,
        storage_ptr store);
        
    BOOST_BEAST_DECL
    object(
        std::initializer_list<value_type> init,
        size_type bucket_count,
        storage_ptr store);

    BOOST_BEAST_DECL
    object&
    operator=(object&& other);

    BOOST_BEAST_DECL
    object&
    operator=(object const& other);

    BOOST_BEAST_DECL
    object&
    operator=(
        std::initializer_list<value_type> init);

    BOOST_BEAST_DECL
    storage_ptr const&
    get_storage() const noexcept;

    //--------------------------------------------------------------------------
    //
    // Iterators
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    iterator
    begin() noexcept;

    BOOST_BEAST_DECL
    const_iterator
    begin() const noexcept;

    BOOST_BEAST_DECL
    const_iterator
    cbegin() noexcept;

    BOOST_BEAST_DECL
    iterator
    end()  noexcept;

    BOOST_BEAST_DECL
    const_iterator
    end() const noexcept;

    BOOST_BEAST_DECL
    const_iterator
    cend() noexcept;

    //--------------------------------------------------------------------------
    //
    // Capacity
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    bool
    empty() const noexcept;

    BOOST_BEAST_DECL
    size_type
    size() const noexcept;

    BOOST_BEAST_DECL
    size_type
    max_size() const noexcept;

    //--------------------------------------------------------------------------
    //
    // Modifiers
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    void
    clear() noexcept;

    BOOST_BEAST_DECL
    std::pair<
        iterator, bool>
    insert(value_type&& v);

    BOOST_BEAST_DECL
    std::pair<
        iterator, bool>
    insert(value_type const& v);

    template<class P
#ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            std::is_constructible<value_type,
                P&&>::value>::type
#endif
    >
    std::pair<
        iterator, bool>
    insert(P&& p);

    BOOST_BEAST_DECL
    iterator
    insert(
        const_iterator hint,
        value_type const& v);

    BOOST_BEAST_DECL
    iterator
    insert(
        const_iterator hint,
        value_type&& v);

    template<class P
#ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            std::is_constructible<value_type,
                P&&>::value>::type
#endif
    >
    iterator
    insert(
        const_iterator hint,
        P&& p);

    template<class InputIt>
    void
    insert(InputIt first, InputIt last);

    BOOST_BEAST_DECL
    void
    insert(std::initializer_list<
        value_type> init);

    BOOST_BEAST_DECL
    insert_return_type
    insert(node_type&& nh);

    BOOST_BEAST_DECL
    iterator
    insert(
        const_iterator hint,
        node_type&& nh);

    template<class M>
    std::pair<iterator, bool>
    insert_or_assign(
        key_type key, M&& obj);

    template <class M>
    iterator
    insert_or_assign(
        const_iterator hint,
        key_type key,
        M&& obj);

    template<class... Args>
    std::pair<
        iterator, bool>
    emplace(
        Args&&... args);

    template<class... Args>
    iterator
    emplace_hint(
        const_iterator hint,
        Args&&... args);

    template<class... Args>
    std::pair<iterator, bool>
    try_emplace(
        key_type key,
        Args&&... args);

    template<class... Args>
    std::pair<iterator, bool>
    try_emplace(
        const_iterator hint,
        key_type key,
        Args&&... args);

    BOOST_BEAST_DECL
    iterator
    erase(const_iterator pos);
    
    BOOST_BEAST_DECL
    iterator
    erase(
        const_iterator first,
        const_iterator last);

    BOOST_BEAST_DECL
    size_type
    erase(key_type key);

    BOOST_BEAST_DECL
    void
    swap(object& other) noexcept;

    BOOST_BEAST_DECL
    node_type
    extract(
        const_iterator pos);

    BOOST_BEAST_DECL
    node_type
    extract(key_type key);

    BOOST_BEAST_DECL
    void
    merge(object& source);

    BOOST_BEAST_DECL
    void
    merge(object&& source);

    //--------------------------------------------------------------------------
    //
    // Lookup
    //
    //--------------------------------------------------------------------------
    
    BOOST_BEAST_DECL
    value&
    at(key_type key);
    
    BOOST_BEAST_DECL
    value const&
    at(key_type key) const;

    BOOST_BEAST_DECL
    value&
    operator[](key_type key);
    
    BOOST_BEAST_DECL
    value const&
    operator[](key_type key) const;

    BOOST_BEAST_DECL
    size_type
    count(key_type key) const;

    BOOST_BEAST_DECL
    size_type
    count(
        key_type key,
        std::size_t hash) const;

    BOOST_BEAST_DECL
    iterator
    find(key_type key);

    BOOST_BEAST_DECL
    iterator
    find(
        key_type key,
        std::size_t hash);

    BOOST_BEAST_DECL
    const_iterator
    find(key_type key) const;

    BOOST_BEAST_DECL
    const_iterator
    find(
        key_type key,
        std::size_t hash) const;

    BOOST_BEAST_DECL
    bool
    contains(key_type key) const;

    BOOST_BEAST_DECL
    bool
    contains(
        key_type key,
        std::size_t hash ) const;

    BOOST_BEAST_DECL
    std::pair<iterator, iterator>
    equal_range(key_type key);

    BOOST_BEAST_DECL
    std::pair<iterator,iterator>
    equal_range(
        key_type key,
        std::size_t hash);

    BOOST_BEAST_DECL
    std::pair<
        const_iterator,
        const_iterator>
    equal_range(key_type key) const;

    BOOST_BEAST_DECL
    std::pair<
        const_iterator,
        const_iterator>
    equal_range(
        key_type key,
        std::size_t hash) const;

    //--------------------------------------------------------------------------
    //
    // Bucket Interface
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    local_iterator
    begin(size_type n) noexcept;

    BOOST_BEAST_DECL
    const_local_iterator
    begin(size_type n) const noexcept;

    BOOST_BEAST_DECL
    const_local_iterator
    cbegin(size_type n) noexcept;

    BOOST_BEAST_DECL
    local_iterator
    end(size_type n)  noexcept;

    BOOST_BEAST_DECL
    const_local_iterator
    end(size_type n) const noexcept;

    BOOST_BEAST_DECL
    const_local_iterator
    cend(size_type n) noexcept;

    BOOST_BEAST_DECL
    size_type
    bucket_count() const noexcept;

    BOOST_BEAST_DECL
    size_type
    max_bucket_count() const noexcept;

    BOOST_BEAST_DECL
    size_type
    bucket_size(size_type n) const noexcept;

    BOOST_BEAST_DECL
    size_type
    bucket(key_type key) const noexcept;

    //--------------------------------------------------------------------------
    //
    // Hash Policy
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    float
    load_factor() const noexcept;

    BOOST_BEAST_DECL
    float
    max_load_factor() const;

    BOOST_BEAST_DECL
    void
    max_load_factor(float ml);

    BOOST_BEAST_DECL
    void
    rehash(size_type count);

    BOOST_BEAST_DECL
    void
    reserve(size_type count);

    //--------------------------------------------------------------------------
    //
    // Observers
    //
    //--------------------------------------------------------------------------

    BOOST_BEAST_DECL
    hasher
    hash_function() const;

    BOOST_BEAST_DECL
    key_equal
    key_eq() const;

private:
    BOOST_BEAST_DECL
    static
    size_type
    constrain_hash(
        std::size_t hash,
        size_type bucket_count) noexcept;

    BOOST_BEAST_DECL
    element*
    find_element(
        key_type key,
        std::size_t hash) const noexcept;

    BOOST_BEAST_DECL
    std::pair<iterator, bool>
    emplace_impl(value_type&& v);

    BOOST_BEAST_DECL
    std::pair<iterator, bool>
    emplace_impl(value_type const& v);

    template<class... Args>
    std::pair<iterator, bool>
    emplace_impl(
        key_type key,
        Args&&... args);

    BOOST_BEAST_DECL
    element*
    prepare_insert(
        key_type key,
        std::size_t hash);

    BOOST_BEAST_DECL
    void
    prepare_inserts(
        size_type count);

    BOOST_BEAST_DECL
    void
    finish_insert(
        element* e, std::size_t hash);

    BOOST_BEAST_DECL
    void
    remove(element* e);

    BOOST_BEAST_DECL
    void
    destroy_elements() noexcept;

    BOOST_BEAST_DECL
    void
    copy(object const& other);
};

} // json
} // beast
} // boost

// Must be included here for this file to stand alone
#include <boost/beast/_experimental/json/value.hpp>

// headers for this file are at the bottom of value.hpp

#endif
