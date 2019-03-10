#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

file (GLOB_RECURSE APP_FILES . *.*)

source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${APP_FILES})

add_definitions (-DBOOST_BEAST_SPLIT_COMPILATION)

add_executable (lounge-server
    ${APP_FILES}
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    ${NLOHMANN_FILES}
    )

if(NOT WIN32)
  find_package (Threads)
  target_link_libraries (lounge-server PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY})
endif()

set_target_properties (lounge-server PROPERTIES
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set_property (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY
  VS_STARTUP_PROJECT lounge-server)
