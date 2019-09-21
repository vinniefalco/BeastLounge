#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})

GroupSources(test/beast/core "/")

add_executable (beast-core-tests
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    Jamfile
    expected.cpp
)

add_test(beast-core-tests beast-core-tests)

target_link_libraries (beast-core-tests
    lib-asio
    lib-beast
    lib-test
)
