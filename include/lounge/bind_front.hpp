//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_BIND_FRONT_HPP
#define LOUNGE_BIND_FRONT_HPP

#include <lounge/config.hpp>
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <type_traits>
#include <utility>

namespace lounge {

namespace detail {

template<class T>
struct shared_handler
{
    boost::shared_ptr<T> self;

    template<class... Args>
    void
    operator()(Args&&... args) const
    {
        (*self)(std::forward<Args>(args)...);
    }
};

template<class T>
struct handler
{
    T* self;

    template<class... Args>
    void
    operator()(Args&&... args) const
    {
        (*self)(std::forward<Args>(args)...);
    }
};

} // detail

template<
    class T
    ,class = typename std::enable_if<
        std::is_base_of<
            boost::enable_shared_from,
            T>::value>::type
>
detail::shared_handler<T>
bind_front(T* this_)
{
    return {boost::shared_from(this_)};
}

template<
    class T
    ,class = typename std::enable_if<
        ! std::is_base_of<
            boost::enable_shared_from,
            T>::value>::type
>
detail::handler<T>
bind_front(T* this_)
{
    return {this_};
}

} // lounge

#endif
