//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BOOST_BEAST_JSON_IMPL_BASIC_PARSER_IPP
#define BOOST_BEAST_JSON_IMPL_BASIC_PARSER_IPP

#include <boost/beast/_experimental/json/basic_parser.hpp>
#include <boost/beast/_experimental/json/error.hpp>
#include <boost/assert.hpp>

namespace boost {
namespace beast {
namespace json {

/*  References:

    https://www.json.org/

    RFC 7159: The JavaScript Object Notation (JSON) Data Interchange Format
    https://tools.ietf.org/html/rfc7159
*/

enum class basic_parser::state : char
{
    json,
    element,
    ws,
    value,

    object1, object2, object3, object4, colon,

    array1,  array2,  array3,  array4,

    key1,    key2,
    string1, string2, string3,

    true1,   true2,   true3,   true4,
    false1,  false2,  false3,  false4,  false5,
    null1,   null2,   null3,   null4,

    number,         number_mant1,       number_mant2,
    number_fract1,  number_fract2,      number_fract3,
    number_exp,     number_exp_sign,
    number_exp_digits1, number_exp_digits2, number_end,

    end
};

basic_parser::
basic_parser()
{
    reset();
}

bool
basic_parser::is_done() const noexcept
{
    return stack_.back() == state::end;
}

void
basic_parser::
reset()
{
    stack_.clear();
    stack_.push_back(state::end);
    stack_.push_back(state::json);
    key_.clear();
    n_ = {};
}

//------------------------------------------------------------------------------

// Append the digit to the
// value, which must be unsigned.
// Returns `false` on overflow.
template<class Unsigned>
bool
basic_parser::
append_digit(
    Unsigned* value,
    char digit)
{
    Unsigned temp = *value * 10;
    if(temp < *value)
        return false;
    Unsigned result = temp + digit;
    if(result < temp)
        return false;
    *value = result;
    return true;
}

//------------------------------------------------------------------------------

void
basic_parser::
write_eof(error_code& ec)
{
    auto const fail =
        [this, &ec]
        {
            char c = 0;
            write(boost::asio::const_buffer(&c, 1), ec);
            BOOST_ASSERT(ec);
        };

    while(stack_.back() != state::end)
    {
        // pop all states that
        // allow "" (empty string)
        switch(stack_.back())
        {
        case state::number_mant2:
        case state::number_fract1:
        case state::number_fract3:
        case state::number_exp:
        case state::number_exp_digits2:
            stack_.back() = state::number_end;
            write(boost::asio::const_buffer(), ec);
            if(ec)
                return;
            break;

        case state::ws:
            stack_.pop_back();
            break;

        default:
            return fail();
        }
    }
    ec = {};
}

//------------------------------------------------------------------------------

std::size_t
basic_parser::
write(boost::asio::const_buffer buffer, error_code& ec)
{
    auto p = static_cast<char const*>(buffer.data());
    auto n = buffer.size();
    auto const p0 = p;
    auto const p1 = p0 + n;
    ec.assign(0, ec.category());
    BOOST_ASSERT(stack_.back() != state::end);
loop:
    switch(stack_.back())
    {
    case state::json:
        this->on_document_begin(ec);
        if(ec)
            goto finish;
        stack_.back() = state::element;
        goto loop;

    case state::element:
        stack_.back() = state::ws;
        stack_.push_back(state::value);
        stack_.push_back(state::ws);
        goto loop;

    case state::ws:
        while(p < p1)
        {
            if(! is_ws(*p))
            {
                stack_.pop_back();
                goto loop;
            }
            ++p;
        }
        break;

    case state::value:
    {
        if(p >= p1)
            break;
        switch(*p)
        {
        // object
        case '{':
            ++p;
            stack_.back() = state::object1;
            this->on_object_begin(ec);
            if(ec)
                goto finish;
            key_.clear();
            goto loop;

        // array
        case '[':
            ++p;
            stack_.back() = state::array1;
            this->on_array_begin(ec);
            key_.clear();
            goto loop;

        // string
        case '"':
            ++p;
            stack_.back() = state::string2;
            goto loop;

        // number
        case '0':
        case '1': case '2': case '3':
        case '4': case '5': case '6':
        case '7': case '8': case '9':
        case '-':
            stack_.back() = state::number;
            goto loop;

        // true
        case 't':
            if(p + 4 <= p1)
            {
                if(
                    p[1] != 'r' ||
                    p[2] != 'u' ||
                    p[3] != 'e')
                {
                    ec = error::syntax;
                    goto finish;
                }
                p = p + 4;
                stack_.back() = state::true4;
                goto loop;
            }
            ++p;
            stack_.back() = state::true1;
            goto loop;

        // false
        case 'f':
            if(p + 5 <= p1)
            {
                if(
                    p[1] != 'a' ||
                    p[2] != 'l' ||
                    p[3] != 's' ||
                    p[4] != 'e')
                {
                    ec = error::syntax;
                    goto finish;
                }
                p = p + 5;
                stack_.back() = state::false5;
                goto loop;
            }
            ++p;
            stack_.back() = state::false1;
            goto loop;

        // null
        case 'n':
            if(p + 4 <= p1)
            {
                if(
                    p[1] != 'u' ||
                    p[2] != 'l' ||
                    p[3] != 'l')
                {
                    ec = error::syntax;
                    goto finish;
                }
                p = p + 4;
                stack_.back() = state::null4;
                goto loop;
            }
            ++p;
            stack_.back() = state::null1;
            goto loop;

        default:
            ec = error::syntax;
            goto finish;
        }
        break;
    }

    //
    // object
    //

    case state::object1:
        stack_.back() = state::object2;
        stack_.push_back(state::ws);
        goto loop;

    case state::object2:
        if(p >= p1)
            break;
        if(*p == '}')
        {
            ++p;
            stack_.back() = state::object4;
            goto loop;
        }
        if(*p != '"')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::object3;
        stack_.push_back(state::element);
        stack_.push_back(state::colon);
        stack_.push_back(state::ws);
        stack_.push_back(state::key2);
        key_.clear();
        goto loop;

    case state::object3:
        if(p >= p1)
            break;
        if(*p == '}')
        {
            ++p;
            stack_.back() = state::object4;
            goto loop;
        }
        if(*p != ',')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::object3;
        stack_.push_back(state::element);
        stack_.push_back(state::colon);
        stack_.push_back(state::ws);
        stack_.push_back(state::key1);
        stack_.push_back(state::ws);
        goto loop;

    case state::object4:
        this->on_object_end(ec);
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    case state::colon:
        if(p >= p1)
            break;
        if(*p != ':')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.pop_back();
        goto loop;

    //
    // array
    //

    case state::array1:
        stack_.back() = state::array2;
        stack_.push_back(state::ws);
        goto loop;

    case state::array2:
        if(p >= p1)
            break;
        if(*p == ']')
        {
            ++p;
            stack_.back() = state::array4;
            goto loop;
        }
        stack_.back() = state::array3;
        stack_.push_back(state::element);
        goto loop;

    case state::array3:
        if(p >= p1)
            break;
        if(*p == ']')
        {
            ++p;
            stack_.back() = state::array4;
            goto loop;
        }
        if(*p != ',')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::array3;
        stack_.push_back(state::element);
        stack_.push_back(state::ws);
        goto loop;

    case state::array4:
        this->on_array_end(ec);
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    //
    // key
    //

    case state::key1:
        if(p >= p1)
            break;
        if(*p != '"')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::key2;
        key_.clear();
        goto loop;

    case state::key2:
    {
        auto const first = p;
        while(p < p1)
        {
            if(*p == '"')
            {
                key_.append(first, p);
                ++p;
                stack_.pop_back();
                goto loop;
            }
            if(is_control(*p))
            {
                // illegal character in key
                ec = error::syntax;
                goto finish;
            }
            ++p;
        }
        key_.append(first, p);
        break;
    }

    //
    // string
    //

    case state::string1:
        if(p >= p1)
            break;
        if(*p != '"')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::string2;
        goto loop;

    case state::string2:
        this->on_string_begin(ec);
        if(ec)
            goto finish;
        stack_.back() = state::string3;
        goto loop;

    case state::string3:
    {
        auto const first = p;
        while(p < p1)
        {
            if(*p == '"')
            {
                this->on_string_piece(
                    string_view(first, p - first), ec);
                if(ec)
                    goto finish;
                this->on_string_end(ec);
                key_.clear();
                if(ec)
                    goto finish;
                ++p;
                stack_.pop_back();
                goto loop;
            }
            if(is_control(*p))
            {
                ec = error::syntax;
                goto finish;
            }
            ++p;
        }
        break;
    }

    //
    // true
    //

    case state::true1:
        if(p >= p1)
            break;
        if(*p != 'r')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::true2;
        BOOST_FALLTHROUGH;

    case state::true2:
        if(p >= p1)
            break;
        if(*p != 'u')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::true3;
        BOOST_FALLTHROUGH;

    case state::true3:
        if(p >= p1)
            break;
        if(*p != 'e')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::true4;
        BOOST_FALLTHROUGH;

    case state::true4:
        this->on_bool(true, ec);
        key_.clear();
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    //
    // false
    //

    case state::false1:
        if(p >= p1)
            break;
        if(*p != 'a')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::false2;
        BOOST_FALLTHROUGH;

    case state::false2:
        if(p >= p1)
            break;
        if(*p != 'l')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::false3;
        BOOST_FALLTHROUGH;

    case state::false3:
        if(p >= p1)
            break;
        if(*p != 's')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::false4;
        BOOST_FALLTHROUGH;

    case state::false4:
        if(p >= p1)
            break;
        if(*p != 'e')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::false5;
        BOOST_FALLTHROUGH;

    case state::false5:
        this->on_bool(false, ec);
        key_.clear();
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    //
    // null
    //

    case state::null1:
        if(p >= p1)
            break;
        if(*p != 'u')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::null2;
        BOOST_FALLTHROUGH;

    case state::null2:
        if(p >= p1)
            break;
        if(*p != 'l')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::null3;
        BOOST_FALLTHROUGH;

    case state::null3:
        if(p >= p1)
            break;
        if(*p != 'l')
        {
            ec = error::syntax;
            goto finish;
        }
        ++p;
        stack_.back() = state::null4;
        BOOST_FALLTHROUGH;

    case state::null4:
        this->on_null(ec);
        key_.clear();
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    //
    // number
    //

    case state::number:
        BOOST_ASSERT(p < p1);
        n_ = {};
        if(*p == '-')
        {
            ++p;
            n_.neg = true;
        }
        stack_.back() = state::number_mant1;
        goto loop;

    case state::number_mant1:
        if(p >= p1)
            break;
        if(! is_digit(*p))
        {
            // expected mantissa digit
            ec = error::syntax;
            goto finish;
        }
        if(*p != '0')
        {
            stack_.back() = state::number_mant2;
            goto loop;
        }
        ++p;
        stack_.back() = state::number_fract1;
        goto loop;

    case state::number_mant2:
        while(p < p1)
        {
            if(! is_digit(*p))
            {
                stack_.back() = state::number_fract1;
                goto loop;
            }
            if(! append_digit(&n_.mant, *p++ - '0'))
            {
                ec = error::mantissa_overflow;
                goto finish;
            }
        }
        break;

    case state::number_fract1:
        if(p >= p1)
            break;
        if(*p == '.')
        {
            ++p;
            stack_.back() = state::number_fract2;
            goto loop;
        }
        if(is_digit(*p))
        {
            // unexpected digit after zero
            ec = error::syntax;
            goto finish;
        }
        stack_.back() = state::number_exp;
        goto loop;

    case state::number_fract2:
        if(p >= p1)
            break;
        if(! is_digit(*p))
        {
            // expected mantissa fraction digit
            ec = error::syntax;
            goto finish;
        }
        stack_.back() = state::number_fract3;
        goto loop;

    case state::number_fract3:
        while(p < p1)
        {
            if(! is_digit(*p))
            {
                stack_.back() = state::number_exp;
                goto loop;
            }
            if(! append_digit(&n_.mant, *p++ - '0'))
            {
                ec = error::mantissa_overflow;
                goto finish;
            }
            n_.pos = false;
            ++n_.exp;
        }
        break;

    case state::number_exp:
        if(p >= p1)
            break;
        if(*p == 'e' || *p == 'E')
        {
            ++p;
            stack_.back() = state::number_exp_sign;
            goto loop;
        }
        stack_.back() = state::number_end;
        goto loop;

    case state::number_exp_sign:
        if(p >= p1)
            break;
        if(*p == '+')
        {
            ++p;
        }
        if(*p == '-')
        {
            ++p;
            n_.pos = false;
        }
        stack_.back() = state::number_exp_digits1;
        goto loop;

    case state::number_exp_digits1:
        if(p >= p1)
            break;
        if(! is_digit(*p))
        {
            // expected exponent digit
            ec = error::syntax;
            goto finish;
        }
        stack_.back() = state::number_exp_digits2;
        goto loop;

    case state::number_exp_digits2:
        while(p < p1)
        {
            if(! is_digit(*p))
            {
                stack_.back() = state::number_end;
                goto loop;
            }
            if(! append_digit(&n_.exp, *p++ - '0'))
            {
                ec = error::exponent_overflow;
                goto finish;
            }
        }
        break;

    case state::number_end:
        this->on_number(n_, ec);
        key_.clear();
        if(ec)
            goto finish;
        stack_.pop_back();
        goto loop;

    //
    // (end)
    //

    case state::end:
        /*
        if(p < p1)
        {
            // unexpected extra characters
            ec = error::syntax;
            goto finish;
        }
        */
        break;
    }

finish:
    return p - p0;
}

} // json
} // beast
} // boost

#endif
