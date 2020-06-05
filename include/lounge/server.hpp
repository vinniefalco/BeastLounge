//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_SERVER_HPP
#define LOUNGE_SERVER_HPP

#include <lounge/config.hpp>
#include <lounge/executor_type.hpp>
#include <lounge/log.hpp>
#include <lounge/rpc.hpp>
#include <lounge/service.hpp>
#include <lounge/detail/key_type.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json/value.hpp>
#include <memory>
#include <typeindex>

namespace lounge {

/** A server instance.

    Order of events during lifetime:

    1. Construction
    2. Insert services in an order-dependent fashion (services
       can look up other services they depend on here).
    3. The caller submits administrative RPC commands to
       configure the services (at this point, no more services
       may be added)
    4. server::run() is invoked:
       - service::on_start() is called for each service
       - io_context::run() is called from each I/O thread
    5. When the server gets the signal to stop:
       - service::on_stop() is called for each service
    6. When io_context::run() returns from the thread that
       called server::run that thread calls join on each I/O thread.
*/
class server
{
public:
    LOUNGE_DECL
    virtual ~server() = 0;

    /** Return a new server instance.
    */
    LOUNGE_DECL
    static
    std::unique_ptr<server>
    create();

    //------------------------------------------------------

    /** Create a service object and add it to the @ref server.
    */
    template<
        class Service,
        class... Args>
    friend
    Service&
    emplace_service(
        server& srv,
        Args&&... args);

    /** Return a reference to the specified service.

        @throw std::runtime_error Service not found.
    */
    template<class Service>
    friend
    Service&
    get_service(
        server& srv);

    /** Return a log section, creating it if necessary.
    */
    virtual
    log&
    get_log(
        string_view name) = 0;

    //------------------------------------------------------

    /** Return a new executor to use for I/O.
    */
    virtual
    executor_type
    make_executor() = 0;

    /** Return `true` if the start() was called.
    */
    virtual
    bool
    is_started() const noexcept = 0;

    /** Run the server.

        This call will block until the server is fully stopped.
        After the server has stopped, the only valid operation
        on the server is destruction.

        @par Effects

        @li @ref service::on_start is called for each
            installed service.

        @li `io_context::run` is called for the specified number
            of I/O threads, including the caller's thread.

        @param threads The number of threads to use for I/O,
        including the caller's thread.
    */
    virtual
    void
    run(int threads = 0) = 0;

    /** Stop the server.

        This will cancel all existing I/O, close all sockets,
        and cause @ref run to eventually return.
    */
    virtual
    void
    stop() = 0;

    //------------------------------------------------------

    /** Add an RPC command to the server.

        The handler must be invokable with
        this equivalent signature:
        @code
        void handler( rpc_response&, json::value const& )
        @endcode

    */
    template<class Handler>
    void
    add_rpc(
        string_view method,
        Handler&& handler);

    /** Add an RPC command to the server.
    */
    template<class Service>
    void
    add_rpc(
        string_view method,
        void (Service::*mf)(
            rpc_response&,
            json::value const&),
        Service* svc);

    /** Execute an RPC command with admin privileges.
    */
    virtual
    void
    do_rpc(
        string_view method,
        json::object params = {}) = 0;

protected:
    struct rpc_handler;

    virtual
    void
    add_rpc_impl(
        string_view method,
        std::unique_ptr<rpc_handler>) = 0;

    virtual
    void*
    emplace_service_impl(
        std::type_index ti,
        std::unique_ptr<service> sp) = 0;

    virtual
    void*
    get_service_impl(
        std::type_index ti) = 0;
};

} // lounge

#include <lounge/impl/server.hpp>

#endif
