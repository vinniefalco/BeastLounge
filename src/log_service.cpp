//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "log_service.hpp"
#include <boost/beast/_experimental/unit_test/dstream.hpp>
#include <boost/container/set.hpp>
#include <iostream>
#include <mutex>
#include <string>
#include <type_traits>
#include <utility>

namespace lounge {

namespace {

class log_service_impl;

struct thread_info
{
    //std::string 
};

class log_impl : public log
{
    log_service_impl& svc_;
    std::string name_;
    int thresh_ = 0;

public:
    log_impl(
        log_service_impl& svc,
        string_view name)
        : svc_(svc)
        , name_(name.to_string())
    {
    }

    string_view
    name() const noexcept
    {
        return name_;
    }

    void
    prepare(
        int level, std::ostream& os) override
    {
        os << name_ << "\t" << level << "\t";
    }

    void
    do_write(
        string_view s) override;

    int
    threshold() const noexcept override
    {
        return thresh_;
    }
};

//----------------------------------------------------------

struct less
{
    using is_transparent = std::true_type;

    bool
    operator()(
        log_impl const& lhs,
        log_impl const& rhs) const noexcept
    {
        return lhs.name() < rhs.name();
    }
};

//----------------------------------------------------------

class log_service_impl : public log_service
{
    server& srv_;
    beast::unit_test::dstream cerr_;
    boost::container::set<
        log_impl, less> logs_;
    std::mutex m_;
    std::string buf_;

public:
    log_service_impl(
        server& srv)
        : srv_(srv)
        , cerr_(std::cerr)
    {
    }

    void
    on_start() override
    {
    }

    void
    on_stop() override
    {
    }

    log&
    get_log(
        string_view name) override
    {
        return *logs_.emplace(
            *this, name).first;
    }

    void
    write(
        string_view s)
    {
        {
            std::lock_guard<
                std::mutex> lock(m_);
            buf_.append(
                s.data(), s.size());
        }

        // VFALCO hack for now,
        // flush on every write
        flush();
    }

    void
    flush()
    {
        std::string buf;
        {
            std::lock_guard<
                std::mutex> lock(m_);
            buf = std::move(buf_);
        }
        cerr_ << buf;
        cerr_.flush();
    }
};

//----------------------------------------------------------

void
log_impl::
do_write(
    string_view s)
{
    svc_.write(s);
}

} // (anon)

//----------------------------------------------------------

log_service&
log_service::
install(
    server& srv)
{
    return emplace_service<
        log_service_impl>(srv);
}

} // lounge
