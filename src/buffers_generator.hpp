//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_BUFFERS_GENERATOR_HPP
#define SRC_BUFFERS_GENERATOR_HPP

#include <lounge/config.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/compose.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/beast/core/buffer_traits.hpp>
#include <cstdlib>
#include <type_traits>

namespace lounge {

template<class T, class = void>
struct is_buffers_generator
    : std::false_type
{
};

template<class T>
struct is_buffers_generator<
    T, detail::void_t<decltype(
        typename T::const_buffers_type(
            std::declval<T&>().prepare(
                std::declval<error_code&>())),
        std::declval<T&>().consume(
            std::size_t{})
    )>> : std::true_type
{
};

/* Exemplar:

// A buffer sequence generator
struct BuffersGenerator
{
    using const_buffers_type = __implementation_defined__

    const_buffers_type prepare( error_code& ec );
    void consume( std::size_t n );
};

static_assert(
    is_buffers_generator<BuffersGenerator>::value, "");
*/

//----------------------------------------------------------

namespace detail {

template<
    class AsyncWriteStream,
    class BuffersGenerator>
struct write_buffers_generator_op
    : boost::asio::coroutine
{
    AsyncWriteStream& stream_;
    BuffersGenerator generator_;
    std::size_t total_ = 0;

    write_buffers_generator_op(
        AsyncWriteStream& stream,
        BuffersGenerator generator)
        : stream_(stream)
        , generator_(std::move(generator))
    {
    }

    template<class Self>
    void
    operator()(
        Self& self,
        error_code ec = {},
        std::size_t n = 0)
    {
        BOOST_ASIO_CORO_REENTER(*this)
        {
            for(;;)
            {
                BOOST_ASIO_CORO_YIELD
                {
                    auto cb =
                        generator_.prepare(ec);
                    if(ec)
                        break;
                    if(beast::buffer_bytes(cb) == 0)
                        break;
                    stream_.async_write_some(
                        cb, std::move(self));
                }
                generator_.consume(n);
                if(ec)
                    break;

                total_ += n;
            }

            self.complete(ec, total_);
        }
    }
};

} // detail

//----------------------------------------------------------

template<
    class AsyncWriteStream,
    class BuffersGenerator,
    class CompletionToken
#if 1
    ,typename std::enable_if<
        is_buffers_generator<
            BuffersGenerator
                >::value>::type* = nullptr
#endif
>
auto
async_write(
    AsyncWriteStream& stream,
    BuffersGenerator generator,
    CompletionToken&& token) ->
        typename net::async_result<
            typename std::decay<CompletionToken>::type,
            void(error_code, std::size_t)>::return_type
{
    return asio::async_compose<
        CompletionToken,
        void(error_code, std::size_t)>(
        detail::write_buffers_generator_op<
            AsyncWriteStream,
            BuffersGenerator>{
                stream,
                std::move(generator)},
            token,
            stream);
}

} // lounge

#endif
