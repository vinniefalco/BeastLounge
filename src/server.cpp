//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "http_service.hpp"
#include "log_service.hpp"
#include "sigint_service.hpp"
#include "string_view_hash.hpp"

#include <lounge/server.hpp>
#include <lounge/service.hpp>
#include <lounge/user_service.hpp>

#include <boost/assert.hpp>

#include <atomic>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <unordered_map>

namespace lounge {

namespace {

//----------------------------------------------------------

class server_impl : public server
{   
    // order matters here

    bool started_ = false;

    std::unordered_map<
        std::type_index,
        std::unique_ptr<service>
            > services_;

    std::unordered_map<
        string_view,
        std::unique_ptr<
            rpc_handler>,
        string_view_hash
            > rpcs_;

    log_service& log_svc_;  // must come after above
    net::io_context ioc_;
    std::atomic<bool> stop_ = false;

    log& log_; // server log

public:
    server_impl()
        : log_svc_(log_service::install(*this))
        , log_(get_log("server"))
    {
        // Order matters here, for services
        // which depend on other services.
        http_service::install(*this);
        user_service::install(*this);
        sigint_service::install(*this);
    }

private:
    log&
    get_log(
        string_view name) override
    {
        return log_svc_.get_log(name);
    }

    executor_type
    make_executor() override
    {
        return net::make_strand(
            ioc_.get_executor());
    }

    bool
    is_started() const noexcept override
    {
        return started_;
    }

    void
    do_run()
    {
        try
        {
            ioc_.run();
            return;
        }
        catch(std::exception const& e)
        {
            LOG_FTL(log_,
                "unhandled exception: ",
                e.what());
        }
        catch(...)
        {
            LOG_FTL(log_,
                "unhandled exception");
        }
        stop();
    }

    void
    run(int threads) override
    {
        BOOST_ASSERT(! started_);

        // start all the services
        for(auto& svc : services_)
            svc.second->on_start();

        started_ = true;

        // VFALCO Do we need to give the services
        //        a way to fail the startup?

        // determine how many threads to run
        if(threads == 0)
        {
            threads = std::thread::hardware_concurrency();
            if(threads == 0)
                threads = 1;
        }

        // run on the specified number of threads
        std::vector<std::thread> vt;
        if(threads > 1)
        {
            vt.reserve(threads - 1);
            while(--threads)
                vt.emplace_back(
                    [this]
                    {
                        this->do_run();
                    });
        }
        do_run();

        // at this point there is no more work.
        for(auto& t : vt)
            t.join();
    }

    void
    stop() override
    {
        if(stop_.exchange(true))
            return;

        // stop all the services
        for(auto& svc : services_)
            svc.second->on_stop();
    }

    //------------------------------------------------------

    void
    do_rpc(
        string_view method,
        json::object params) override
    {
        json::object obj(params.storage());
        obj.emplace("jsonrpc", "2.0");
        obj.emplace("method", method);
        if(! params.empty())
        {
            obj.emplace("params", std::move(params));
        }
        do_one_rpc(std::move(obj));
    }

    void
    do_one_rpc(
        json::value jv)
    {
        rpc_response res;

        auto const& obj =
            jv.as_object();
        auto const& method =
            obj.at("method").as_string();
        auto const& params =
            obj.at("params").as_object();
        auto it = rpcs_.find(method.subview());
        if(it == rpcs_.end())
        {
            // error
        }
        else
        {
            it->second->invoke(res, params);
        }
    }

#if 0
    {
        // Can only issue RPC after starting
        BOOST_ASSERT(is_started());

        auto it = services_by_name_.find(
            target);
        if(it == services_by_name_.end())
        {
            // not found
        }
    }
#endif

private:
    virtual
    void
    add_rpc_impl(
        string_view method,
        std::unique_ptr<
            rpc_handler> p) override
    {
        rpcs_.emplace(
            method,
            std::move(p));
    }

    void*
    emplace_service_impl(
        std::type_index ti,
        std::unique_ptr<service> sp) override
    {
        // Cannot add services after starting.
        BOOST_ASSERT(! started_);
        auto r = services_.emplace(
            ti, std::move(sp));
        if(! r.second)
            throw std::runtime_error(
                "service already exists");
        return r.first->second.get();
    }

    void*
    get_service_impl(
        std::type_index ti) override
    {
        auto it = services_.find(ti);
        if(it != services_.end())
            return it->second.get();
        throw std::runtime_error(
            "service not found");
    }
};

} // (anon)

//----------------------------------------------------------

server::
~server()
{
}

std::unique_ptr<server>
server::
create()
{
    return std::unique_ptr<server>(
        new server_impl);
}

} // lounge
