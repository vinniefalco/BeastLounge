//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SERVER_HPP
#define LOUNGE_SERVER_HPP

#include "config.hpp"
#include "logger.hpp"
#include <boost/beast/core/basic_stream.hpp>
#include <boost/beast/core/error.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/system_executor.hpp>
#include <memory>

//------------------------------------------------------------------------------

namespace detail {

template<class T>
struct self_handler
{
    std::shared_ptr<T> self;

    template<class... Args>
    void
    operator()(Args&&... args)
    {
        (*self)(std::forward<Args>(args)...);
    }
};

} // detail

/** Return an invocable handler for `this_`.

    This work-around for C++11 missing lambda-capture expression
    helps with implementations that use macro-based coroutines.
*/
template<class T>
detail::self_handler<T>
self(T* this_)
{
    return {this_->shared_from_this()};
}

//------------------------------------------------------------------------------

/** An instance of the lounge server.
*/
class server
{
public:
    /// The type of executor agents and sessions will use
    using executor_type = net::strand<
        net::system_executor>;

    /// The type of socket for agents to use
    using socket_type =
        net::basic_stream_socket<tcp, executor_type>;

    /// The type of plain stream for agents to use
    using stream_type =
        beast::basic_stream<tcp, executor_type>;

    /** Base for polymorphic services

        The lifetime of each agent is the
        same as the server lifetime.
    */
    struct agent
    {
        virtual ~agent() = default;

        /** Called when the server starts.

            This will be called at most once.
        */
        virtual
        void
        on_start() = 0;

        /** Called when the server stops.

            This will be called at most once.
        */
        virtual
        void
        on_stop() = 0;
    };

    virtual ~server() = default;

    /** Return a new executor to use.
    */
    virtual
    executor_type
    make_executor() = 0;

    /** Add an agent to the server.

        Agents may only be added before calling start().
    */
    virtual
    void
    insert(
        std::shared_ptr<agent> sp) = 0;

    /** Run the server.

        This call will block until the server is fully stopped.
        After the server has stopped, the only valid operation
        on the server is destruction.
    */
    virtual
    void
    run() = 0;

    /// Return the log object
    virtual
    logger&
    log() noexcept = 0;
};

//------------------------------------------------------------------------------

/** Create a server.

    The configuration file is loaded,
    and all child objects are created.
*/
extern
std::unique_ptr<server>
make_server(
    char const* config_path,
    std::unique_ptr<logger> log);

#endif
