//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef LOUNGE_USER_SERVICE_HPP
#define LOUNGE_USER_SERVICE_HPP

#include <lounge/server.hpp>
#include <lounge/user.hpp>
#include <memory>

namespace lounge {

class user_service
    : public service
{
public:
    /** Create a new user.
    */
    template<class Handler>
    boost::shared_ptr<user>
    create_user(
        Handler&& h);

    /** Create a new user data key using the type T.
    */
    //template<class T>
    
    static
    user_service&
    install(
        server& srv);

protected:
    using handler = user::handler;

    virtual
    boost::shared_ptr<user>
    create_user_impl(
        std::unique_ptr<handler>) = 0;
};

} // lounge

#include <lounge/impl/user_service.hpp>

#endif
