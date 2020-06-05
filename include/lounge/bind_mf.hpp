//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_BIND_MF_HPP
#define LOUNGE_BIND_MF_HPP

#include <lounge/config.hpp>
#include <utility>

namespace lounge {

namespace detail {

template<class T, class FP>
struct bind_mf_wrapper
{
    T* t_;
    FP fp_;

    template<class... Args>
    void
    operator()(Args&&... args) const
    {
        (t_->*fp_)(std::forward<Args>(args)...);
    }
};

} // detail

/** Call wrapper for a member function.
*/
template<class T, class FP>
detail::bind_mf_wrapper<T, FP>
bind_mf(FP fp, T* t)
{
    return { t, fp };
}

} // lounge

#endif
