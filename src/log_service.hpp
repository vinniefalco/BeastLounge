//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#ifndef SRC_LOG_SERVICE_HPP
#define SRC_LOG_SERVICE_HPP

#include <lounge/log.hpp>
#include <lounge/server.hpp>
#include <memory>

namespace lounge {

/** Handles logging.
*/
class log_service
    : public service
{
public:
    virtual ~log_service() = default;

    virtual
    log&
    get_log(
        string_view name) = 0;

    static
    log_service&
    install(
        server& srv);
};

} // lounge

#endif
