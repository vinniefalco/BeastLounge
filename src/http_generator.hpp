//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_HTTP_GENERATOR_HPP
#define SRC_HTTP_GENERATOR_HPP

#include <lounge/config.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/serializer.hpp>
#include <boost/smart_ptr/make_shared.hpp>

namespace lounge {

//----------------------------------------------------------

class http_generator
{
public:
    class const_buffers_type
    {
        net::const_buffer const* begin_ = nullptr;
        net::const_buffer const* end_ = nullptr;

    public:
        using value_type = net::const_buffer;

        const_buffers_type() = default;
        const_buffers_type(
            const_buffers_type const&) = default;
        const_buffers_type&
        operator=(
            const_buffers_type const&) = default;

        const_buffers_type(
            net::const_buffer const* p,
            std::size_t n)
            : begin_(p)
            , end_(p + n)
        {
        }

        value_type const*
        begin() const noexcept
        {
            return begin_;
        }

        value_type const*
        end() const noexcept
        {
            return end_;
        }
    };

    template<
        bool isRequest,
        class Body,
        class Fields>
    http_generator(
        http::message<
            isRequest, Body, Fields>&& m)
        : impl_(boost::make_shared<
            generator_impl<
                isRequest, Body, Fields>>(
                    std::move(m)))
    {
    }

    const_buffers_type
    prepare(
        error_code& ec)
    {
        return impl_->prepare(ec);
    }

    void
    consume(
        std::size_t n)
    {
        impl_->consume(n);
    }

    bool
    keep_alive() const noexcept
    {
        return impl_->keep_alive();
    }

private:
    struct generator
    {
        virtual
        ~generator() = default;
        
        virtual
        const_buffers_type
        prepare(
            error_code& ec) = 0;

        virtual
        void
        consume(
            std::size_t n) = 0;

        virtual
        bool
        keep_alive() const noexcept = 0;
    };

    template<
        bool isRequest,
        class Body,
        class Fields>
    struct generator_impl : generator
    {
        static std::size_t constexpr max_buf = 16;

        http::message<
            isRequest, Body, Fields> m_;
        http::serializer<
            isRequest, Body, Fields> sr_;
        net::const_buffer buf_[max_buf];

        explicit
        generator_impl(http::message<
            isRequest, Body, Fields>&& m)
            : m_(std::move(m))
            , sr_(m_)
        {
        }

        struct visit
        {
            generator_impl* this_;
            const_buffers_type& cb_;

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

        const_buffers_type
        prepare(error_code& ec) override
        {
            if(sr_.is_done())
                return {};
            const_buffers_type cb;
            sr_.next(ec, visit{this, cb});
            return cb;
        }

        void
        consume(std::size_t n) override
        {
            sr_.consume(n);
        }

        bool
        keep_alive() const noexcept override
        {
            return m_.keep_alive();
        }
    };

    boost::shared_ptr<generator> impl_;
};

} // lounge

#endif
