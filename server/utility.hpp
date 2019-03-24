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
#include <boost/smart_ptr/enable_shared_from_this.hpp>
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

struct enable_shared_from
    : boost::enable_shared_from_this<enable_shared_from>
{
};

template<class T>
boost::shared_ptr<T>
shared_from(T* p)
{
    return boost::shared_ptr<T>(
        p->shared_from_this(), p);
}

template<class T>
boost::weak_ptr<T>
weak_from(T* p)
{
#if 0
    return boost::weak_ptr<T>(
        p->weak_from_this(), p);
#else
    return boost::weak_ptr<T>(
        boost::static_pointer_cast<T>(
            p->weak_from_this().lock()));
#endif
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

template<class T, class MFn>
struct shared_mfn_handler
{
    MFn mfn_;
    boost::shared_ptr<T> self;

    template<class... Args>
    void
    operator()(Args&&... args) const
    {
        ((*self).*mfn_)(std::forward<Args>(args)...);
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

template<class T, class MFn>
struct mfn_handler
{
    MFn mfn_;
    T* self;

    template<class... Args>
    void
    operator()(Args&&... args) const
    {
        ((*self).*mfn_)(std::forward<Args>(args)...);
    }
};

} // detail

template<class T
    ,class = typename std::enable_if<
        std::is_base_of<enable_shared_from, T>::value>::type
>
detail::shared_handler<T>
bind_front(T* this_)
{
    return {shared_from(this_)};
}

template<class T, class MFn
    ,class = typename std::enable_if<
        std::is_base_of<enable_shared_from, T>::value>::type
>
detail::shared_mfn_handler<T, MFn>
bind_front(T* this_, MFn mfn)
{
    return {mfn, shared_from(this_)};
}

template<class T
    ,class = typename std::enable_if<
        ! std::is_base_of<enable_shared_from, T>::value>::type
>
detail::handler<T>
bind_front(T* this_)
{
    return {this_};
}

template<class T, class MFn
    ,class = typename std::enable_if<
        ! std::is_base_of<enable_shared_from, T>::value>::type
>
detail::mfn_handler<T, MFn>
bind_front(T* this_, MFn mfn)
{
    return {mfn, this_};
}

//------------------------------------------------------------------------------

template<class T, class U>
boost::weak_ptr<T>
weak_ptr_cast(boost::weak_ptr<U> const& wp) noexcept
{
    return boost::static_pointer_cast<T>(wp.lock());
}

#endif
