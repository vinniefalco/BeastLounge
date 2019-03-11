//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_ALLOCATOR_HPP
#define BOOST_BEAST_JSON_ALLOCATOR_HPP

#include <boost/beast/core/detail/config.hpp>
#include <boost/beast/_experimental/json/storage.hpp>
#include <type_traits>

namespace boost {
namespace beast {
namespace json {

template<typename T>
class allocator
{
    storage_ptr sp_;

    template<typename U>
    friend class allocator;

public:
    using value_type = T;

    allocator(allocator const&) = default;
    allocator& operator=(allocator const&) = default;

    allocator(allocator&&) noexcept;

    allocator& operator=(allocator&&) noexcept;

    template<typename U>
    allocator(allocator<U> const& other) noexcept;

    explicit
    allocator(storage_ptr sp) noexcept;

    T*
    allocate(size_t n);

    void
    deallocate(T* t, size_t n) noexcept;

    bool
    operator==(allocator const&) const noexcept;

    bool
    operator!=(allocator const&) const noexcept;

    storage_ptr
    get_storage() const & noexcept
    {
        return sp_;
    }

    storage_ptr
    get_storage() && noexcept
    {
        return std::move(sp_);
    }
};

} // json
} // beast
} // boost

#include <boost/beast/_experimental/json/impl/allocator.hpp>

#endif
