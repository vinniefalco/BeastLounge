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

struct server::rpc
{
    virtual ~rpc() = default;
    virtual void invoke() const = 0;
};

template<class Handler>
void
server::
add_rpc(
    string_view method,
    Handler&& handler)
{
    struct rpc_impl : rpc
    {
        Handler h;

        explicit
        rpc_impl(Handler&& h_)
            : h(std::forward<
                Handler>(h_))
        {
        }

        void
        invoke() const override
        {
            h();
        }
    };

    this->add_rpc_impl(
        method,
        std::unique_ptr<rpc_impl>(
            new rpc_impl(std::forward<
                Handler>(handler))));
}

template<class Service>
void
server::
add_rpc(
    string_view method,
    void (Service::*mf)(),
    Service* svc)
{
    add_rpc(method,
        [svc, mf]()
        {
            (svc->*mf)();
        });
}

} // lounge

#endif
