//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_MAKE_MESSAGE_HPP
#define SRC_MAKE_MESSAGE_HPP

#include <lounge/config.hpp>
#include "buffer_stream.hpp"
#include "const_buffer_range.hpp"
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/serializer.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <cstdlib>

namespace lounge {

//----------------------------------------------------------

namespace detail {

template<
    bool isRequest,
    class Body,
    class Fields>
struct message_stream_impl
    : buffer_stream
{
    static std::size_t constexpr max_buf = 16;

    http::message<
        isRequest, Body, Fields> m_;
    http::serializer<
        isRequest, Body, Fields> sr_;
    net::const_buffer buf_[max_buf];

    explicit
    message_stream_impl(
        http::message<
            isRequest, Body, Fields>&& m)
        : m_(std::move(m))
        , sr_(m_)
    {
    }

    struct visit
    {
        message_stream_impl* this_;
        const_buffer_range& cb_;

        template<
            class ConstBufferSequence>
        void
        operator()(
            error_code&,
            ConstBufferSequence const& buffers)
        {
            auto it =
                net::buffer_sequence_begin(
                    buffers);
            auto const end =
                net::buffer_sequence_end(
                    buffers);
            std::size_t n = 0;
            while(it != end && n < max_buf)
                this_->buf_[n++] = *it++;
            cb_ = { this_->buf_, n };
        }
    };

    const_buffer_range
    prepare() override
    {
        if(sr_.is_done())
            return {};
        const_buffer_range cb;
        error_code ec;
        sr_.next(ec, visit{this, cb});
        return cb;
    }

    void
    consume(std::size_t n) override
    {
        sr_.consume(n);
    }
};

} // detail

template<
    bool isRequest,
    class Body,
    class Fields>
boost::shared_ptr<buffer_stream>
make_message(
    http::message<
        isRequest, Body, Fields>&& m)
{
    return boost::make_shared<
        detail::message_stream_impl<
            isRequest, Body, Fields>>(
                std::move(m));
}

} // lounge

#endif
