//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "logger.hpp"
#include <boost/beast/_experimental/unit_test/dstream.hpp>
#include <boost/beast/core/file.hpp>
#include <boost/container/set.hpp>
#include <boost/make_unique.hpp>
#include <iostream>
#include <mutex>
#include <set>

//------------------------------------------------------------------------------

namespace {

class logger_impl : public logger
{
    beast::unit_test::dstream cerr_;
    logger_config cfg_;
    beast::file file_;
    std::mutex m_;

    struct hash;

    class section_impl : public section
    {
        friend struct hash;

        logger_impl& log_;
        std::string name_;
        int thresh_ = 0;

    public:
        section_impl(
            logger_impl& log,
            beast::string_view name)
            : log_(log)
            , name_(name.to_string())
        {
        }

        beast::string_view
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
            beast::string_view s) override
        {
            std::lock_guard<
                std::mutex> lock(log_.m_);
            beast::error_code ec;
            log_.file_.write(
                s.data(), s.size(), ec);
            // VFALCO what about ec?
        }

        int
        threshold() const noexcept
        {
            return thresh_;
        }
    };

    struct less
    {
        using is_transparent = std::true_type;

        bool
        operator()(
            section_impl const& lhs,
            section_impl const& rhs) const noexcept
        {
            return lhs.name() < rhs.name();
        }
    };

    boost::container::set<section_impl, less> sections_;

    //--------------------------------------------------------------------------

    std::ostream&
    cerr() override
    {
        return cerr_;
    }

    bool
    open(logger_config cfg) override
    {
        cfg_ = std::move(cfg);

        beast::error_code ec;
        file_.open(
            cfg_.path.c_str(),
            beast::file_mode::append,
            ec);
        if(ec)
        {
            cerr_ <<
                "logger::open \"" <<
                cfg_.path << "\": " <<
                ec.message() << "\n";
            return false;
        }

        return true;
    }

    section&
    get_section(beast::string_view name) override
    {
        auto result =
            sections_.emplace(*this, name);
        return *result.first;
    }

public:
    explicit
    logger_impl()
        : cerr_(std::cerr)
    {
    }
};

} // (anon)

//------------------------------------------------------------------------------

/** Create the logger object
*/
std::unique_ptr<logger>
make_logger()
{
    return boost::make_unique<logger_impl>();
}
