#
# Copyright (c) 2018 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

set (TEST_MAIN ${Boost_INCLUDE_DIRS}/boost/beast/_experimental/unit_test/main.cpp)

file (GLOB APP_FILES
    Jamfile
    empty.cpp
)

file (GLOB_RECURSE JSON_FILES
  ${PROJECT_SOURCE_DIR}/include/nlohmann/*.hpp
)

source_group (TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/boost/beast PREFIX beast FILES ${BEAST_EXTRA_FILES})
source_group (TREE ${PROJECT_SOURCE_DIR}/include/nlohmann PREFIX nlohmann FILES ${NLOHMANN_FILES})
source_group ("" FILES ${TEST_MAIN})
source_group ("" FILES ${APP_FILES})

add_executable (tests
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    ${JSON_FILES}
    ${TEST_MAIN}
    ${APP_FILES}
)
