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

GroupSources(test/beast/json "/")

add_executable (beast-json-tests
    ${BEAST_FILES}
    ${BEAST_EXTRA_FILES}
    Jamfile
    _detail_stack.cpp
    allocator.cpp
    array.cpp
    assign_string.cpp
    assign_vector.cpp
    basic_parser.cpp
    error.cpp
    iterator.cpp
    json.cpp
    key_param.cpp
    kind.cpp
    number.cpp
    object.cpp
    parse_file.cpp
    parser.cpp
    serializer.cpp
    src.cpp
    storage.cpp
    string.cpp
    value.cpp
)

target_link_libraries (beast-json-tests
    lib-asio
    lib-beast
    lib-beast-extra
    lib-test
    )

