#
# Copyright (c) 2018 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

file (GLOB APP_FILES
  beast.hpp
  http_session.cpp
  http_session.hpp
  listener.cpp
  listener.hpp
  main.cpp
  net.hpp
  shared_state.cpp
  shared_state.hpp
  websocket_session.cpp
  websocket_session.hpp
  chat_client.html
  README.md
)

source_group ("" FILES ${APP_FILES})
source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

add_executable (lounge-server ${APP_FILES} ${BEAST_FILES})

if(NOT WIN32)
  find_package (Threads)
  target_link_libraries (lounge-server PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
endif()

set_target_properties (lounge-server PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT lounge-server)
