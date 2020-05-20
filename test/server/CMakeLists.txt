#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})

GroupSources(test/server "/")

include_directories (${PROJECT_SOURCE_DIR}/server)
include_directories (${PROJECT_SOURCE_DIR}/test)

add_executable (server-tests
    ${PROJECT_SOURCE_DIR}/test/test_suite.hpp
    ${PROJECT_SOURCE_DIR}/test/main.cpp
    blackjack.cpp
    message_test.cpp
)
target_link_libraries (server-tests
    Boost::json
    lib-asio
    lib-beast
    lib-test
)
