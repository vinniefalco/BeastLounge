#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

set (TEST_MAIN ${Boost_INCLUDE_DIRS}/boost/beast/_experimental/unit_test/main.cpp)

source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})
source_group ("beast-test" FILES ${TEST_MAIN})

GroupSources(test/server "/")

add_definitions (-DBOOST_BEAST_SPLIT_COMPILATION)
 
include_directories (${PROJECT_SOURCE_DIR}/server)

add_executable (server-tests
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    message_test.cpp
)

target_link_libraries (server-tests
    lib-asio
    lib-beast
    lib-beast-extra
    lib-test
)