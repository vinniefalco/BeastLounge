//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_LOG_HPP
#define LOUNGE_LOG_HPP

#include <lounge/config.hpp>
#include <memory>
#include <ostream>
#include <sstream>

namespace lounge {

class log
{
    template<class T1, class T2, class... TN>
    static
    void
    append(
        std::ostream& os,
        T1 const& t1,
        T2 const& t2,
        TN const&... tn)
    {
        os << t1;
        append(os, t2, tn...);
    }

    template<class T>
    static
    void
    append(
        std::ostream& os,
        T const& t)
    {
        os << t;
    }

    virtual void prepare(
        int level, std::ostream& os) = 0;

    virtual void do_write(
        string_view s) = 0;

public:
    LOUNGE_DECL
    virtual ~log() = 0;

    virtual
    int
    threshold() const noexcept = 0;

    template<class... Args>
    void
    write(int level, Args const&... args)
    {
        std::string s;
        {
            std::stringstream ss;
            prepare(level, ss);
            append(ss, args...);
            ss << '\n';
            s = ss.str();
        }
        do_write(s);
    }
};

} // lounge

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

#endif
