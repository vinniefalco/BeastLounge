//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_NUMBER_IPP
#define BOOST_BEAST_JSON_IMPL_NUMBER_IPP

#include <boost/beast/_experimental/json/number.hpp>
#include <boost/beast/core/static_string.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace boost {
namespace beast {
namespace json {

number::
number(
    mantissa_type mant,
    exponent_type exp,
    bool negative) noexcept
    : mant_(mant)
    , exp_(exp)
    , neg_(negative)
{
    //normalize();
}

//------------------------------------------------------------------------------

number::
number(short v) noexcept
{
    assign(v);
}

number::
number(int v) noexcept
{
    assign(v);
}

number::
number(long v) noexcept
{
    assign(v);
}

number::
number(long long v) noexcept
{
    assign(v);
}

number::
number(unsigned short v) noexcept
{
    assign(v);
}

number::
number(unsigned int v) noexcept
{
    assign(v);
}

number::
number(unsigned long v) noexcept
{
    assign(v);
}

number::
number(unsigned long long v) noexcept
{
    assign(v);
}

number::
number(float v) noexcept
{
    assign(v);
}

number::
number(double v) noexcept
{
    assign(v);
}

number::
number(long double v) noexcept
{
    assign(v);
}

//------------------------------------------------------------------------------

bool
number::
is_int64() const noexcept
{
    if(exp_ != 0)
        return false;
    if(neg_)
        return mant_ <= static_cast<
            mantissa_type>(
                (std::numeric_limits<
                    std::int64_t>::max)()) + 1;
    return mant_ <= static_cast<
        mantissa_type>(
            (std::numeric_limits<
                std::int64_t>::max)());
}

bool
number::
is_uint64() const noexcept
{
    if(exp_ != 0)
        return false;
    return true;
}

std::int_least64_t
number::
get_int64() const noexcept
{
    if(neg_)
        return -static_cast<
            std::int_least64_t>(mant_);
    return static_cast<
        std::int_least64_t>(mant_);
}

std::uint_least64_t
number::
get_uint64() const noexcept
{
    return mant_;
}

long double
number::
get_double() const noexcept
{
    static exponent_type const bias =
        static_cast<exponent_type>(std::floor(
            std::log10((std::numeric_limits<
                mantissa_type>::max)()))) - 2;
    long double v =
        static_cast<long double>(mant_);
    if(exp_ < -bias)
    {
        v = (v * std::pow(10, -bias)) *
            std::pow(10, exp_ + bias);
    }
    else
    {
        v = v * std::pow(10, exp_);
    }
    if(neg_)
        return -v;
    return v;
}

bool
operator==(
    number const& lhs,
    number const& rhs) noexcept
{
    return
        lhs.mantissa() == rhs.mantissa() &&
        lhs.exponent() == rhs.exponent() &&
        lhs.is_negative() == rhs.is_negative();
}

bool
operator!=(
    number const& lhs,
    number const& rhs) noexcept
{
    return
        lhs.mantissa() != rhs.mantissa() ||
        lhs.exponent() != rhs.exponent() ||
        lhs.is_negative() != rhs.is_negative();
}

string_view
number::
print(char* dest) const noexcept
{
    if(mant_ == 0)
    {
        BOOST_ASSERT(exp_ == 0);
        BOOST_ASSERT(! neg_);
        *dest = '0';
        return { dest, 1 };
    }
    auto const start = dest;
    auto const tab = pow10::get();
    auto dig =
        std::upper_bound(
            tab.begin(),
            tab.end(),
            mant_) -
        tab.begin();
    if(neg_)
        *dest++ = '-';
    auto rp = dest + dig;
    dest = rp;
    auto mant = mant_;
    while(mant > 0)
    {
        *--rp = '0' + mant % 10;
        mant /= 10;
    }
    auto exp = static_cast<int>(exp_);
    if(exp != 0)
    {
        *dest++ = 'e';
        if(exp < 0)
        {
            *dest++ = '-';
            exp = std::abs(exp);
        }
    }
    dig =
        std::upper_bound(
            tab.begin(),
            tab.end(),
            exp) -
        tab.begin();
    rp = dest + dig;
    dest = rp;
    while(exp > 0)
    {
        *--rp = '0' + exp % 10;
        exp /= 10;
    }
    return { start, static_cast<
        std::size_t>(dest - start) };
}

//------------------------------------------------------------------------------

auto
number::
pow10::
get() noexcept ->
    pow10 const&
{
    struct pow10_impl : pow10
    {
        pow10_impl()
        {
            static mantissa_type constexpr list[] = {
                1ULL,
                10ULL,
                100ULL,
                1000ULL,
                10000ULL,
                100000ULL,
                1000000ULL,
                10000000ULL,
                100000000ULL,
                1000000000ULL,
                10000000000ULL,
                100000000000ULL,
                1000000000000ULL,
                10000000000000ULL,
                100000000000000ULL,
                1000000000000000ULL,
                10000000000000000ULL,
                100000000000000000ULL,
                1000000000000000000ULL,
                10000000000000000000ULL
            };
            size_ = std::extent<
                decltype(list)>::value;
            begin_ = &list[0];
            end_ = &list[size_];
        }
    };

    static pow10_impl const tab;
    return tab;
}

void
number::
normalize() noexcept
{
    if(mant_ == 0)
        return;
    while(mant_ % 10 == 0)
    {
        mant_ /= 10;
        ++exp_;
    }
}

template<class I>
void
number::
assign(I v
    ,typename std::enable_if<
        std::is_signed<I>::value &&
        std::is_integral<I>::value
            >::type*) noexcept
{
    BOOST_STATIC_ASSERT(
        sizeof(I) <= sizeof(mantissa_type));
    if(v >= 0)
    {
        neg_ = false;
        mant_ = v;
    }
    else if(v != (std::numeric_limits<I>::min)())
    {
        neg_ = true;
        mant_ = static_cast<
            mantissa_type>(std::abs(v));
    }
    else
    {
        neg_ = true;
        mant_ = (std::numeric_limits<I>::max)();
        mant_ += 1;
    }
    exp_ = 0;
}

template<class U>
void
number::
assign(U v
    ,typename std::enable_if<
        std::is_unsigned<U>::value
            >::type*) noexcept
{
    BOOST_STATIC_ASSERT(
        sizeof(U) <= sizeof(mantissa_type));
    mant_ = v;
    exp_ = 0;
    neg_ = false;
}

template<class F>
void
number::
assign(F v
    ,typename std::enable_if<
        std::is_floating_point<
        F>::value>::type*) noexcept
{
/*
    Ryu's algorithm is Boost licensed and performs
    this operation with more speed and more precision:

    https://github.com/ulfjack/ryu/blob/41910f966fdc2307a279ef645de976e0fc431b48/ryu/d2s.c#L211
*/
    if(v < 0)
    {
        v = std::abs(v);
        neg_ = true;
    }
    else
    {
        neg_ = false;
    }
    static exponent_type const bias =
        static_cast<exponent_type>(std::floor(
            std::log10((std::numeric_limits<
                mantissa_type>::max)()))) - 2;
    exp_ = static_cast<exponent_type>(
        std::floor(std::log10(v)));
    auto mant = static_cast<long double>(v) /
        std::pow(10, exp_);
    mant_ = static_cast<mantissa_type>(
        mant * std::pow(10, bias));
    exp_ -= bias;
    normalize();
}

//------------------------------------------------------------------------------

std::ostream&
operator<<(std::ostream& os, number const& n)
{
    char buf[number::max_string_chars];
    os << n.print(buf);
    return os;
}

} // json
} // beast
} // boost

#endif
