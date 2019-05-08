//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_UTILITY_HPP
#define LOUNGE_UTILITY_HPP

#include "config.hpp"
#include <boost/smart_ptr/enable_shared_from.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <chrono>
#include <type_traits>

template<class T>
struct is_duration : std::false_type
{
};

template<class Rep, class Period>
struct is_duration<
    std::chrono::duration<Rep, Period>> : std::true_type
{
};
 
template<
    class To, class Rep, class Period,
    class = typename std::enable_if<
        is_duration<To>::value>::type
>
To
ceil(std::chrono::duration<Rep, Period> const& d)
{
    To t = std::chrono::duration_cast<To>(d);
    if (t < d)
        return t + To{1};
    return t;
}

//------------------------------------------------------------------------------

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

template<class T
    ,class = typename std::enable_if<std::is_base_of<
        boost::enable_shared_from, T>::value>::type
>
detail::shared_handler<T>
bind_front(T* this_)
{
    return {boost::shared_from(this_)};
}

template<class T
    ,class = typename std::enable_if<! std::is_base_of<
        boost::enable_shared_from, T>::value>::type
>
detail::handler<T>
bind_front(T* this_)
{
    return {this_};
}

#endif
