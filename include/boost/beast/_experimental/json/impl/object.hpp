//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_OBJECT_HPP
#define BOOST_BEAST_JSON_IMPL_OBJECT_HPP

#include <boost/beast/_experimental/json/value.hpp>
#include <boost/beast/_experimental/json/detail/varint.hpp>
#include <boost/core/exchange.hpp>
#include <algorithm>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

//------------------------------------------------------------------------------

struct object::list_hook
{
public:
    element* prev_;
    element* next_;
};

struct object::element
    : public list_hook
{
    value v_;
    element* local_next_;

    BOOST_BEAST_DECL
    string_view
    key() const noexcept;

    BOOST_BEAST_DECL
    static
    element*
    allocate(
        storage_ptr const& sp,
        value_type const& v);

    BOOST_BEAST_DECL
    static
    element*
    allocate(
        storage_ptr const& sp,
        value_type const& v,
        storage_ptr const&);

    BOOST_BEAST_DECL
    static
    element*
    allocate(
        storage_ptr const& sp,
        value_type&& v);

    BOOST_BEAST_DECL
    static
    element*
    allocate(
        storage_ptr const& sp,
        value_type&& v,
        storage_ptr const&);

    struct cleanup
    {
        std::size_t size;
        storage_ptr const& sp;
        std::size_t n;

        void
        operator()(char* p)
        {
            sp->deallocate(p,
                size, alignof(element));
        }
    };

    template<class... Args>
    static
    element*
    allocate(
        storage_ptr const& sp,
        key_type key,
        Args&&... args)
    {
        auto up =
            prepare_allocate(sp, key);
        auto const p = up.get();
        auto const n = up.get_deleter().n;
        ::new(up.get()) element(
            std::forward<Args>(args)...);
        up.release();
        detail::varint_write(
            p + sizeof(element), key.size());
        std::memcpy(
            p + sizeof(element) + n,
            key.data(),
            key.size());
        p[sizeof(element) +
            n + key.size()] = '\0';
        return reinterpret_cast<element*>(p);
    }

    BOOST_BEAST_DECL
    static
    void
    destroy(
        element const* e,
        storage_ptr const& sp);

    // varint key size
    // char[] key name follows

private:
    template<class... Args>
    explicit
    element(Args&&... args)
        : v_(std::forward<Args>(args)...)
    {
    }

    BOOST_BEAST_DECL
    static
    std::unique_ptr<char, cleanup>
    prepare_allocate(
        storage_ptr const& sp,
        key_type key);
};

//------------------------------------------------------------------------------

class object::hasher
{
    BOOST_BEAST_DECL
    static
    std::pair<
        std::uint64_t, std::uint64_t>
    init(std::true_type) noexcept;

    BOOST_BEAST_DECL
    static
    std::pair<
        std::uint32_t, std::uint32_t>
    init(std::false_type) noexcept;

public:
    BOOST_BEAST_DECL
    std::size_t
    operator()(key_type key) const noexcept;
};

class object::key_equal
{
public:
    bool
    operator()(
        beast::string_view lhs,
        beast::string_view rhs) const noexcept
    {
        return lhs == rhs;
    }
};

//------------------------------------------------------------------------------

class object::pointer
{
    reference t_;

public:
    pointer(reference const& t)
        : t_(t)
    {
    }

    reference*
    operator->() noexcept
    {
        return &t_;
    }
};

class object::const_pointer
{
    const_reference t_;

public:
    const_pointer(
        const_reference const& t)
        : t_(t)
    {
    }

    const_reference*
    operator->() noexcept
    {
        return &t_;
    }
};

//------------------------------------------------------------------------------

class object::iterator
{
    element* e_ = nullptr;

    friend class object;

    iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::pointer;
    using reference = object::reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    iterator() = default;

    iterator(
        iterator const&) = default;

    iterator& operator=(
        iterator const&) = default;

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    bool
    operator==(
        const_iterator const& other) const noexcept;

    bool
    operator!=(
        const_iterator const& other) const noexcept;

    iterator&
    operator++() noexcept
    {
        e_ = e_->next_;
        return *this;
    }

    iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    iterator&
    operator--() noexcept
    {
        e_ = e_->prev_;
        return *this;
    }

    iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    pointer
    operator->() const noexcept
    {
        return reference{
            e_->key(), e_->v_ };
    }

    reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v_ };
    }
};

//------------------------------------------------------------------------------

class object::const_iterator
{
    element* e_ = nullptr;

    friend class object;

    const_iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    using pointer = object::const_pointer;
    using reference = object::const_reference;
    using difference_type = std::ptrdiff_t;
    using iterator_category =
        std::bidirectional_iterator_tag;

    const_iterator() = default;

    const_iterator(
        const_iterator const&) = default;

    const_iterator& operator=(
        const_iterator const&) = default;

    const_iterator(iterator it)
        : e_(it.e_)
    {
    }

    const_iterator&
    operator=(iterator it) noexcept
    {
        e_ = it.e_;
    }

    bool
    operator==(
        const_iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        const_iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    const_iterator&
    operator++() noexcept
    {
        e_ = e_->next_;
        return *this;
    }

    const_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_iterator&
    operator--() noexcept
    {
        e_ = e_->prev_;
        return *this;
    }

    const_iterator
    operator--(int) noexcept
    {
        auto tmp = *this;
        --*this;
        return tmp;
    }

    const_pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v_ };
    }

    const_reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v_ };
    }
};

inline
bool
object::
iterator::
operator==(
    const_iterator const& other) const noexcept
{
    return e_ == other.e_;
}

inline
bool
object::
iterator::
operator!=(
    const_iterator const& other) const noexcept
{
    return e_ != other.e_;
}

//------------------------------------------------------------------------------

class object::local_iterator
{
    element* e_ = nullptr;

    friend class object;

    local_iterator(element* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;

    using reference = object::reference;

    local_iterator() = default;

    local_iterator(
        local_iterator const&) = default;

    local_iterator& operator=(
        local_iterator const&) = default;

    bool
    operator==(
        local_iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        local_iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    local_iterator&
    operator++() noexcept
    {
        e_ = e_->local_next_;
        return *this;
    }

    local_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v_ };
    }

    const_reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v_ };
    }
};

//------------------------------------------------------------------------------

class object::const_local_iterator
{
    element const* e_ = nullptr;

    friend class object;

    const_local_iterator(element const* e)
        : e_(e)
    {
    }

public:
    using value_type = object::value_type;
    
    using reference = object::const_reference;

    const_local_iterator() = default;

    const_local_iterator(
        const_local_iterator const&) = default;

    const_local_iterator& operator=(
        const_local_iterator const&) = default;

    bool
    operator==(
        const_local_iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        const_local_iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    bool
    operator==(
        iterator const& other) const noexcept
    {
        return e_ == other.e_;
    }

    bool
    operator!=(
        iterator const& other) const noexcept
    {
        return e_ != other.e_;
    }

    const_local_iterator&
    operator++() noexcept
    {
        e_ = e_->local_next_;
        return *this;
    }

    const_local_iterator
    operator++(int) noexcept
    {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    const_pointer
    operator->() const noexcept
    {
        return const_reference{
            e_->key(), e_->v_ };
    }

    const_reference
    operator*() const noexcept
    {
        return {
            e_->key(), e_->v_ };
    }
};

//------------------------------------------------------------------------------

class object::node_type
{
    element* e_ = nullptr;
    storage_ptr sp_;

    friend class object;

    node_type(
        element* e,
        storage_ptr sp)
        : e_(e)
        , sp_(std::move(sp))
    {
    }

public:
    node_type() = default;
    node_type(node_type const&) = delete;

    ~node_type()
    {
        if(e_)
            element::destroy(e_, sp_);
    }

    node_type(node_type&& other)
        : e_(boost::exchange(
            other.e_, nullptr))
        , sp_(boost::exchange(
            other.sp_, nullptr))
    {
    }

    storage_ptr const&
    get_storage() const noexcept
    {
        return sp_;
    }

    beast::string_view
    key() const noexcept
    {
        return e_->key();
    }

    json::value&
    value() noexcept
    {
        return e_->v_;
    }

    json::value const&
    value() const noexcept
    {
        return e_->v_;
    }
};

struct object::insert_return_type
{
    iterator position;
    bool inserted;
    node_type node;
};


//------------------------------------------------------------------------------

template<class InputIt>
object::
object(
    InputIt first,
    InputIt last)
    : object(
        first, last,
        std::distance(first, last),
        get_default_storage_ptr())
{
}

template<class InputIt>
object::
object(
    InputIt first,
    InputIt last,
    size_type bucket_count)
    : object(
        first, last,
        bucket_count,
        get_default_storage_ptr())
{
}

template<class InputIt>
object::
object(
    InputIt first,
    InputIt last,
    storage_ptr store)
    : object(
        first, last,
        std::distance(first, last),
        std::move(store))
{
}

template<class InputIt>
object::
object(
    InputIt first,
    InputIt last,
    size_type bucket_count,
    storage_ptr store)
    : object(
        bucket_count,
        std::move(store))
{
    insert(first, last);
}

template<class P, class>
auto
object::
insert(P&& p)->
    std::pair<iterator, bool>
{
    return emplace(
        std::forward<P>(p));
}

template<class P, class>
auto
object::
insert(
    const_iterator hint,
    P&& p) ->
        iterator
{
    return emplace_hint(hint,
        std::forward<P>(p));
}

template<class InputIt>
void
object::
insert(InputIt first, InputIt last)
{
    for(auto it = first;
        it != last; ++it)
        insert(std::move(*it));
}

template<class M>
auto
object::
insert_or_assign(
    key_type key, M&& obj) ->
        std::pair<iterator, bool>
{
}

template<class M>
auto
object::
insert_or_assign(
    const_iterator hint,
    key_type key,
    M&& obj) ->
        iterator
{
}

template<class... Args>
auto
object::
emplace(
    Args&&... args) ->
        std::pair<iterator, bool>
{
    return emplace_impl(
        std::forward<Args>(args)...);
}

template<class... Args>
auto
object::
emplace_hint(
    const_iterator hint,
    Args&&... args) ->
        iterator
{
    boost::ignore_unused(hint);
    return emplace(
        std::forward<Args>(args)...).first;
}

template<class... Args>
auto
object::
try_emplace(
    key_type key,
    Args&&... args) ->
        std::pair<iterator, bool>
{
    return emplace(key,
        std::forward<Args>(args)...);
}

template<class... Args>
auto
object::
try_emplace(
    const_iterator hint,
    key_type key,
    Args&&... args) ->
        std::pair<iterator, bool>
{
    return emplace(hint, key,
        std::forward<Args>(args)...);
}

inline
auto
object::
hash_function() const ->
    hasher
{
    return hasher{};
}

inline
auto
object::
key_eq() const ->
    key_equal
{
    return key_equal{};
}

template<class... Args>
auto
object::
emplace_impl(
    key_type key,
    Args&&... args) ->
        std::pair<iterator, bool>
{
    auto const hash =
        hasher{}(key);
    auto e = prepare_insert(
        key, hash);
    if(e)
        return {e, false};
    e = element::allocate(sp_, key,
        std::forward<Args>(args)...);
    BOOST_ASSERT(
        *e->v_.get_storage() == *sp_);
    finish_insert(e, hash);
    return {e, true};
}

} // json
} // beast
} // boost

#endif
