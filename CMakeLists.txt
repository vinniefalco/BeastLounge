#
# Copyright (c) 2018 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/beast
#

cmake_minimum_required (VERSION 3.13 FATAL_ERROR)
cmake_policy (SET CMP0074 NEW)

project (beast-lounge VERSION 0.1.0 LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (Boost_USE_STATIC_LIBS ON)

if (MSVC)
  set (Boost_USE_STATIC_RUNTIME ON)
  add_definitions (-D_WIN32_WINNT=0x0601)
  add_definitions (-D_SCL_SECURE_NO_WARNINGS=1)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS=1)
  add_definitions (-D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING)
  add_compile_options (/MP)
  string (REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

find_package (Boost REQUIRED COMPONENTS system)
include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

file (GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

file (GLOB_RECURSE BEAST_EXTRA_FILES
  ${PROJECT_SOURCE_DIR}/include/boost/beast/*.hpp
  ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp)

include_directories (${PROJECT_SOURCE_DIR}/include)

add_subdirectory (server)
add_subdirectory (test)

