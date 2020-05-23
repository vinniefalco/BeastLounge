//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_STRING_VIEW_HASH_HPP
#define SRC_STRING_VIEW_HASH_HPP

#include <lounge/config.hpp>
#include <cstdint>

namespace lounge {

// VFALCO temporarily needed until
//   boost::string_view supports std::hash
struct string_view_hash
{
    std::size_t
    operator()(
        string_view s) const
    {
        return (*this)(s,
            std::integral_constant<bool,
            (sizeof(std::size_t)>4)>{});
    }

    std::uint64_t
    operator()(
        string_view s,
        std::true_type) const
    {
        unsigned char const* p =
            reinterpret_cast<
                unsigned char const*>(
                    s.data());
        unsigned char const* const e =
            p + s.size();
        std::uint64_t h =
            14695981039346656037u;
        for (; p < e; ++p)
            h = (h ^ *p) *
                1099511628211u;
        return h;
    }

    std::uint32_t
    operator()(
        string_view s,
        std::false_type) const
    {
        unsigned char const* p =
            reinterpret_cast<
                unsigned char const*>(
                    s.data());
        unsigned char const* const e =
            p + s.size();
        std::uint32_t h =
            2166136261;
        for (; p < e; ++p)
            h = (h ^ *p) *
                16777619;
        return h;
    }
};

} // lounge

#endif
