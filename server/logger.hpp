//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_LOGGER_HPP
#define LOUNGE_LOGGER_HPP

#include "config.hpp"
#include <boost/beast/core/error.hpp>
#include <boost/beast/core/static_string.hpp>
#include <boost/beast/core/string.hpp>
#include <memory>
#include <ostream>
#include <strstream>

struct logger_config
{
    std::string path;
};

//------------------------------------------------------------------------------

class logger
{
public:
    using line_type =
        beast::static_string<2048>;

    class section;

    virtual ~logger() = default;

    /** Return an output stream to the error device.

        The error output device is used for early logging,
        before the log file has been opened.
    */
    virtual
    std::ostream&
    cerr() = 0;

    virtual
    bool
    open(logger_config cfg) = 0;

    virtual
    section&
    get_section(beast::string_view name) = 0;
};

//------------------------------------------------------------------------------

class logger::section
{
    friend class logger;

    template<class T1, class T2, class... TN>
    static
    void
    append(
        line_type& s,
        T1 const& t1,
        T2 const& t2,
        TN const&... tn)
    {
        append(s, t1);
        append(s, t2, tn...);
    }

    template<class T
        ,class = typename std::enable_if<
            ! std::is_integral<T>::value &&
            ! std::is_constructible<
                beast::string_view, T const&>::value>::type>
    static
    void
    append(line_type& s, T const& t)
    {
        std::stringstream ss;
        ss << t;
        append(s, beast::string_view(
            ss.str().data(), ss.str().size()));
    }

    template<class Int>
    static
    typename std::enable_if<
        std::is_integral<Int>::value>::type
    append(line_type& s, Int v)
    {
        auto ss = beast::to_static_string(v);
        append(s, beast::string_view(
            ss.data(), ss.size()));
    }

    static
    void
    append(
        line_type& s,
        beast::string_view v)
    {
        s.append(v.data(),
            (std::min)(v.size(),
                s.max_size() - s.size()));
    }

    virtual void prepare(
        int level, line_type& s) = 0;

    virtual void do_write(
        beast::string_view s) = 0;

public:
    virtual ~section() = default;

    virtual
    int
    threshold() const noexcept = 0;

    template<class... Args>
    void
    write(int level, Args const&... args)
    {
        line_type s;
        try
        {
            prepare(level, s);
            append(s, args...);
        }
        catch(std::length_error const&)
        {
            // line would be truncated
        }
    }
};

#define LOG_AT_LEVEL(sect, level, ...) \
    do { \
        if(level >= sect.threshold()) \
            sect.write(level, __VA_ARGS__); \
    } while(false)
    

/// Log at trace level
#define LOG_TRC(sect, ...) LOG_AT_LEVEL(sect, 0, __VA_ARGS__)

/// Log at debug level
#define LOG_DBG(sect, ...) LOG_AT_LEVEL(sect, 1, __VA_ARGS__)

/// Log at info level (normal)
#define LOG_INF(sect, ...) LOG_AT_LEVEL(sect, 2, __VA_ARGS__)

/// Log at warning level
#define LOG_WRN(sect, ...) LOG_AT_LEVEL(sect, 3, __VA_ARGS__)

/// Log at error level
#define LOG_ERR(sect, ...) LOG_AT_LEVEL(sect, 4, __VA_ARGS__)

/// Log at fatal level
#define LOG_FTL(sect, ...) LOG_AT_LEVEL(sect, 0, __VA_ARGS__)

/** Create the logger object
*/
std::unique_ptr<logger>
make_logger();

#endif
