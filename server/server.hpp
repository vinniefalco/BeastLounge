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
#include "agent.hpp"
#include "logger.hpp"
#include "types.hpp"
#include <boost/beast/_experimental/json/value.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <memory>

//------------------------------------------------------------------------------

namespace detail {

template<class T>
struct self_handler
{
    boost::shared_ptr<T> self;

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
    virtual ~server() = default;

    /// Return the log object
    virtual
    logger&
    log() noexcept = 0;

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
        boost::shared_ptr<agent> sp) = 0;

    /** Run the server.

        This call will block until the server is fully stopped.
        After the server has stopped, the only valid operation
        on the server is destruction.
    */
    virtual
    void
    run() = 0;

    /// Return the document root path
    virtual
    beast::string_view
    doc_root() const = 0;

    /// Return the name of index file for directory
    virtual
    beast::string_view
    index_file() const = 0;

    /// Perform a stat
    virtual
    void
    stat(json::value& jv) = 0;
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
