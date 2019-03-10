//
// Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/BeastLounge
//

#include "listener.hpp"
#include "settings.hpp"
#include "shared_state.hpp"

#include <boost/beast/src.hpp>
#include <boost/beast/src_extra.hpp>

#include <boost/asio/signal_set.hpp>
#include <boost/config.hpp>
#include <iostream>

#ifdef BOOST_MSVC
# ifndef WIN32_LEAN_AND_MEAN // VC_EXTRALEAN
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
# else
#  include <windows.h>
# endif
#endif

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

    // Check command line arguments.
    if (argc != 5)
    {
        std::cerr <<
            "Usage: websocket-chat-server <settings-path> <address> <port> <doc_root>\n" <<
            "Example:\n" <<
            "    websocket-chat-server 0.0.0.0 8080 .\n";
        return EXIT_FAILURE;
    }
    auto settings_path = argv[1];
    auto address = net::ip::make_address(argv[2]);
    auto port = static_cast<unsigned short>(std::atoi(argv[3]));
    auto doc_root = argv[4];

    beast::error_code ec;
    auto st = settings::load_from_file(settings_path, ec);

    // The io_context is required for all I/O
    net::io_context ioc;

    // Create and launch a listening port
    std::make_shared<listener>(
        ioc,
        tcp::endpoint{address, port},
        std::make_shared<shared_state>(doc_root))->run();

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](boost::system::error_code const&, int)
        {
            // Stop the io_context. This will cause run()
            // to return immediately, eventually destroying the
            // io_context and any remaining handlers in it.
            ioc.stop();
        });

    // Run the I/O service on the main thread
    ioc.run();

    // (If we get here, it means we got a SIGINT or SIGTERM)

    return EXIT_SUCCESS;
}
