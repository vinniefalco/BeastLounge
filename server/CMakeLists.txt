#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX _beast FILES ${BEAST_EXTRA_FILES})
GroupSources (server "/")

file (GLOB_RECURSE SERVER_HEADERS . *.hpp)

add_definitions(-DBOOST_JSON_NO_LIB=1)

add_executable (lounge-server
    ${BEAST_EXTRA_FILES}
    ${SERVER_HEADERS}
    Jamfile
    README.md
    core/blackjack.cpp
    core/channel.cpp
    core/channel_list.cpp
    core/http_session.cpp
    core/listener.cpp
    core/logger.cpp
    core/main.cpp
    core/message.cpp
    core/room.cpp
    core/rpc.cpp
    core/server.cpp
    core/system.cpp
    core/user.cpp
    core/ws_user.cpp
    )

target_link_libraries (
    lounge-server PRIVATE
        lib-beast
        lib-asio
        lib-asio-ssl
        Boost::system
        Boost::thread
        Boost::json
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
    )

set_target_properties (lounge-server PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT lounge-server)

include(GNUInstallDirs)

install(
    TARGETS
        lounge-server
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR})
