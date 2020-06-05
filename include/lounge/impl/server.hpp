//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_IMPL_SERVER_HPP
#define LOUNGE_IMPL_SERVER_HPP

namespace lounge {

template<
    class Service,
    class... Args>
Service&
emplace_service(
    server& srv,
    Args&&... args)
{
    using type = typename
        detail::key_type<Service>::type;
    return *reinterpret_cast<Service*>(
        srv.emplace_service_impl(
            std::type_index(typeid(type)),
            std::unique_ptr<service>(
                new Service(srv, std::forward<
                    Args>(args)...))));
}

template<class Service>
Service&
get_service(
    server& srv)
{
    return *reinterpret_cast<
        Service*>(srv.get_service_impl(
            std::type_index(typeid(
                Service))));
}

struct server::rpc_handler
{
    virtual ~rpc_handler() = default;
    virtual void invoke(
        rpc_response&,
        json::value const&) const = 0;
};

template<class Handler>
void
server::
add_rpc(
    string_view method,
    Handler&& handler)
{
    struct rpc_handler_impl : rpc_handler
    {
        Handler h;

        explicit
        rpc_handler_impl(
            Handler&& h_)
            : h(std::forward<
                Handler>(h_))
        {
        }

        void
        invoke(
            rpc_response& res,
            json::value const& params
                ) const override
        {
            h(res, params);
        }
    };

    this->add_rpc_impl(
        method,
        std::unique_ptr<rpc_handler>(
            new rpc_handler_impl(
                std::forward<Handler>(
                    handler))));
}

} // lounge

#endif
