//
// Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include <boost/config.hpp>
#include <lounge/server.hpp>
#include <boost/json/value.hpp>

#ifdef BOOST_MSVC
# ifndef WIN32_LEAN_AND_MEAN // VC_EXTRALEAN
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
# else
#  include <windows.h>
# endif
#endif

#include <stdlib.h>

int
main(int argc, char* argv[])
{
#if BOOST_MSVC
    {
        int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        flags |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(flags);
    }
#endif

    // Create the server.
    auto server = lounge::server::create();

    // (Add additional services)

    // Run config/admin RPC commands

    server->do_rpc(
        "http.create_port",
        {
            { "address" , "127.0.0.1" },
            { "port" , 8080 }
        });

    server->do_rpc(
        "http.set_doc_root",
        {
            { "path" , "C:\\Users\\vinnie\\src\\lounge\\static\\wwwroot" },
        });

    // Run the server on this many threads.
    server->run(1);

    // At this point, the server is stopped.
    return EXIT_SUCCESS;
}
