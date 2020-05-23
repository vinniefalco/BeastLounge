//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_DETAIL_KEY_TYPE_HPP
#define LOUNGE_DETAIL_KEY_TYPE_HPP

namespace lounge {
namespace detail {

template<class T, class = void>
struct key_type
{
    using type = T;
};

template<class T>
struct key_type<T,
    void_t<typename T::key_type>>
{
    using type = typename T::key_type;
};

} // detail
} // lounge

#endif
