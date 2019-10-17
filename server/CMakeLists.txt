#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/json/include/boost/json PREFIX json FILES ${JSON_FILES})
GroupSources (server "/")

file (GLOB_RECURSE SERVER_HEADERS . *.hpp)

add_definitions(-DBOOST_JSON_HEADER_ONLY=1)

add_executable (lounge-server
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    ${JSON_FILES}
    ${SERVER_HEADERS}
    Jamfile
    README.md
    blackjack.cpp
    channel.cpp
    channel_list.cpp
    http_session.cpp
    listener.cpp
    logger.cpp
    main.cpp
    message.cpp
    room.cpp
    rpc.cpp
    server.cpp
    system.cpp
    user.cpp
    ws_user.cpp
    )

target_link_libraries (
    lounge-server PRIVATE
        lib-beast
        lib-asio
        lib-asio-ssl
        Boost::system
        Boost::thread
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
