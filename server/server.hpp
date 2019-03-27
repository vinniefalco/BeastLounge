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
#include "types.hpp"
#include <boost/beast/core/string.hpp>
#include <chrono>
#include <functional>
#include <memory>
#include <utility>

class channel_list;
class logger;
class rpc_handler;
class service;
class user;

//------------------------------------------------------------------------------

/** An instance of the lounge server.
*/
class server
{
public:
    virtual ~server() = default;

    /** Return a new executor to use.
    */
    virtual
    executor_type
    make_executor() = 0;

    /** Add a service to the server.

        Services may only be added before calling start().
    */
    virtual
    void
    insert(
        std::unique_ptr<service> sp) = 0;

    /// Returns `true` if the server is shutting down gracefully
    virtual
    bool
    is_shutting_down() = 0;

    //--------------------------------------------------------------------------
    //
    // Services
    //
    //--------------------------------------------------------------------------

    virtual beast::string_view  doc_root() const = 0;

    virtual logger&             log() = 0;
    virtual ::channel_list&     channel_list() = 0;

    //--------------------------------------------------------------------------

    /** Run the server.

        This call will block until the server is fully stopped.
        After the server has stopped, the only valid operation
        on the server is destruction.
    */
    virtual
    void
    run() = 0;

    /** Shut down the server gracefully
    */
    virtual
    void
    shutdown(std::chrono::seconds cooldown) = 0;

    virtual
    void
    stop() = 0;
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
