//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef LOUNGE_MESSAGE_HPP
#define LOUNGE_MESSAGE_HPP

#include <lounge/config.hpp>
#include <boost/json/value.hpp>
#include <boost/asio/buffer.hpp>
#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>

namespace lounge {

/** A shared buffer sequence of length 1.

    This is a reference counted, copyable handle to a constant
    buffer sequence of length one. It is used for broadcasting.
    The originating buffers are copied upon construction.
*/
class message
{
    struct impl
    {
        net::const_buffer cb;
        std::atomic<std::size_t> count;

        explicit
        impl(std::size_t n)
            : cb(this + 1, n)
            , count(1)
        {
        }
    };

    impl* p_ = nullptr;

    using allocator =
        std::allocator<impl>;

    template<class Buffers>
    impl*
    construct(
        Buffers const& buffers)
    {
        allocator a;
        auto const n =
            net::buffer_size(buffers);
        auto const p = ::new(a.allocate(
            (2 * sizeof(impl) + n - 1) /
                sizeof(impl))) impl(n);
        net::buffer_copy(
            net::mutable_buffer(
                p + 1, n),
            buffers);
        return p;
    }

public:
    using value_type =
        net::const_buffer;

    using iterator =
        value_type const*;

    // Construct a null message
    message() = default;

    // immutable
    message&
    operator=(
        message const&) = delete;

    ~message()
    {
        if(p_ && --p_->count == 0)
        {
            allocator a;
            a.deallocate(p_,
                (2 * sizeof(impl) +
                    p_->cb.size() - 1) /
                sizeof(impl));
        }
    }

    /** Construct a message from a buffer sequence

        This function allocates a flat copy of the input
        buffer sequence.
    */
    template<
        class ConstBufferSequence
#ifndef BOOST_BEAST_DOXYGEN
        ,class = typename std::enable_if<
            net::is_const_buffer_sequence<
                ConstBufferSequence>::value>::type
#endif
    >
    message(
        ConstBufferSequence const& buffers)
        : p_(construct(buffers))
    {
    }

    /** Construct a message from a JSON.
    */
    LOUNGE_DECL
    message(json::value const& jv);

    message(message&& other) noexcept
    {
        p_ = other.p_;
        other.p_ = nullptr;
    }

    message(message const& other) noexcept
        : p_(other.p_)
    {
        if(p_)
            ++p_->count;
    }

    iterator
    begin() const noexcept
    {
        return &p_->cb;
    }

    iterator
    end() const noexcept
    {
        return begin() + 1;
    }

    friend
    void
    swap(
        message& lhs,
        message& rhs) noexcept
    {
        std::swap(
            lhs.p_, rhs.p_);
    }
};

} // lounge

#endif
