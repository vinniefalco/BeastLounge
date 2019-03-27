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

//------------------------------------------------------------------------------

struct number::pow10
{
    std::size_t
    size() const noexcept
    {
        return size_;
    }

    mantissa_type const*
    begin() const noexcept
    {
        return begin_;
    }

    mantissa_type const*
    end() const noexcept
    {
        return end_;
    }

    mantissa_type
    operator[](
        exponent_type n) const
    {
        return begin_[n];
    }
        
    static
    pow10 const&
    get() noexcept
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

protected:
    std::size_t size_;
    mantissa_type const* begin_;
    mantissa_type const* end_;
};

//------------------------------------------------------------------------------

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
number(short i) noexcept
{
    assign_signed(i);
}

number::
number(int i) noexcept
{
    assign_signed(i);
}

number::
number(long i) noexcept
{
    assign_signed(i);
}

number::
number(long long i) noexcept
{
    assign_signed(i);
}

number::
number(unsigned short i) noexcept
{
    assign_unsigned(i);
}

number::
number(unsigned int i) noexcept
{
    assign_unsigned(i);
}

number::
number(unsigned long i) noexcept
{
    assign_unsigned(i);
}

number::
number(unsigned long long i) noexcept
{
    assign_unsigned(i);
}

number::
number(float f) noexcept
{
    assign_ieee(f);
}

number::
number(double f) noexcept
{
    assign_ieee(f);
}

number::
number(long double f) noexcept
{
    assign_ieee(f);
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

void
number::
assign_signed(long long i) noexcept
{
    if(i >= 0)
    {
        neg_ = false;
        mant_ = i;
    }
    else
    {
        neg_ = true;
        mant_ = ~ static_cast<
            mantissa_type>(i) + 1;
    }
    exp_ = 0;
}

void
number::
assign_unsigned(unsigned long long i) noexcept
{
    mant_ = i;
    exp_ = 0;
    neg_ = false;
}

void
number::
assign_ieee(long double f) noexcept
{
/*
    Ryu's algorithm is Boost licensed and performs
    this operation with more speed and more precision:

    https://github.com/ulfjack/ryu/blob/41910f966fdc2307a279ef645de976e0fc431b48/ryu/d2s.c#L211
*/
    if(f < 0)
    {
        f = std::abs(f);
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
        std::floor(std::log10(f)));
    auto mant = f / std::pow(10, exp_);
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
