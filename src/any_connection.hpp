//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_ANY_CONNECTION_HPP
#define SRC_ANY_CONNECTION_HPP

#include <lounge/server.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <memory>
#include <utility>

namespace lounge {

class any_connection
    : public boost::enable_shared_from
{
public:
    /** A list of any connections.
    */
    class list;

    virtual
    ~any_connection();

    /** Close the connection, canceling any pending I/O.
    */
    virtual
    void
    close() = 0;

    list&
    get_list() const noexcept
    {
        return *list_;
    }

private:
    list* list_;
};

//----------------------------------------------------------

/** A list of any connections.
*/
class any_connection::list
{
public:
    virtual ~list() = default;

    /** Return `true` if the list is open.
    */
    virtual
    bool
    is_open() const noexcept = 0;

    /** Close the list.

        This function permanently closes the list:

        * The function @ref any_connection::close
          called for each connection in the list.

        * Subsequent calls to @ref is_open will
          return `false`.

        * Subsequent calls to @ref emplace_any_connection
          will return an null pointer.
    */
    virtual
    void
    close() = 0;

    /** Create a new connection and add it to the list.

        When the connection is destroyed, it will be
        automatically removed from the list.

        @return The new connection, or a null pointer if
        the list is closed.
    */
    template<class T, class... Args>
    friend
    boost::shared_ptr<T>
    emplace_any_connection(
        list& list_,
        Args&&... args);

    /** Return a new, empty list of connections.

        The list is initially open.
    */
    static
    std::unique_ptr<list>
    create();

protected:
    friend class any_connection;

    void
    set_list(
        any_connection& c)
    {
        c.list_ = this;
    }

    virtual
    bool
    insert(any_connection& c) = 0;

    virtual
    void
    erase(any_connection& c) = 0;
};

template<class T, class... Args>
boost::shared_ptr<T>
emplace_any_connection(
    any_connection::list& list,
    Args&&... args)
{
    auto sp = boost::make_shared<T>(
        std::forward<Args>(args)...);
    if( list.insert(*sp))
        return sp;
    return nullptr;
}

} // lounge

#endif
