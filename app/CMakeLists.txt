#
# Copyright (c) 2020 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

file (GLOB_RECURSE LOUNGE_APP_SOURCES . *.hpp, *.cpp)

source_group (TREE ${PROJECT_SOURCE_DIR}/app PREFIX "" FILES ${LOUNGE_APP_SOURCES})

add_definitions(-DBOOST_JSON_NO_LIB=1)
#add_definitions(-DBOOST_ALL_NO_LIB=1)

add_executable (lounge-app
    ${LOUNGE_APP_SOURCES}
    )

target_link_libraries (
    lounge-app PRIVATE
        lounge
        lib-beast
        lib-asio
        lib-asio-ssl
        lib-json
        Boost::disable_autolinking
        #Boost::json
        Boost::system
        Boost::thread
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
    )

set_target_properties (lounge-app PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT lounge-app)

include(GNUInstallDirs)

install(
    TARGETS
        lounge-app
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR})
