#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

cmake_minimum_required (VERSION 3.10 FATAL_ERROR)
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.12.0")
    cmake_policy (SET CMP0074 NEW)
endif()

#-------------------------------------------------------------------------------

function (DoGroupSources curdir rootdir folder)
    file (GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir} ${PROJECT_SOURCE_DIR}/${curdir}/*)
    foreach (child ${children})
        if (IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
            DoGroupSources (${curdir}/${child} ${rootdir} ${folder})
        elseif (${child} STREQUAL "CMakeLists.txt")
            source_group("" FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        else()
            string (REGEX REPLACE ^${rootdir} ${folder} groupname ${curdir})
            string (REPLACE "/" "\\" groupname ${groupname})
            source_group (${groupname} FILES ${PROJECT_SOURCE_DIR}/${curdir}/${child})
        endif()
    endforeach()
endfunction()

function (GroupSources curdir folder)
    DoGroupSources (${curdir} ${curdir} ${folder})
endfunction()

#-------------------------------------------------------------------------------
#
# Beast Lounge
#
#-------------------------------------------------------------------------------

project (beast-lounge VERSION 0.1.0 LANGUAGES CXX)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (Boost_USE_STATIC_LIBS ON)

if (MSVC)
  set (Boost_USE_STATIC_RUNTIME ON)
  set (CMAKE_VERBOSE_MAKEFILE FALSE)

  add_definitions (
    -D_WIN32_WINNT=0x0601
    -D_SCL_SECURE_NO_WARNINGS=1
    -D_CRT_SECURE_NO_WARNINGS=1
    -D_SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
    -D_SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
    )

  # Disable default warnings switch
  string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

  add_compile_options(
    /bigobj       # large object file format
    /permissive-  # strict C++
    /W4           # enable all warnings
    /MP           # Multi-processor compilation
    )

else()
    set (THREADS_PREFER_PTHREAD_FLAG ON)
    find_package (Threads)

    set( CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic -Wno-unused-parameter")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wrange-loop-analysis")
    endif ()

endif()

#-------------------------------------------------------------------------------
#
# Boost / Beast
#
#-------------------------------------------------------------------------------

find_package (Boost REQUIRED COMPONENTS system)
include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

add_definitions (-DBOOST_ASIO_NO_DEPRECATED=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_ARRAY=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_BIND=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_DATE_TIME=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_REGEX=1)
add_definitions (-DBOOST_ASIO_SEPARATE_COMPILATION=1)
add_definitions (-DBOOST_BEAST_SEPARATE_COMPILATION=1)

file (GLOB_RECURSE BEAST_FILES
  ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp
  ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)

file (GLOB_RECURSE BEAST_EXTRA_FILES
  ${PROJECT_SOURCE_DIR}/include/boost/beast/*.hpp
  ${PROJECT_SOURCE_DIR}/include/boost/beast/*.ipp
  ${PROJECT_SOURCE_DIR}/include/boost/beast/*.*
  )

#-------------------------------------------------------------------------------
#
# OpenSSL
#
#-------------------------------------------------------------------------------

if (APPLE AND NOT DEFINED ENV{OPENSSL_ROOT_DIR})
  find_program(HOMEBREW brew)
  if (NOT HOMEBREW STREQUAL "HOMEBREW-NOTFOUND")
    execute_process(COMMAND brew --prefix openssl
      OUTPUT_VARIABLE OPENSSL_ROOT_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE)
  endif()
endif()

find_package(OpenSSL)

#-------------------------------------------------------------------------------
#
# Static Libraries
#
#-------------------------------------------------------------------------------

GroupSources (test "/")

add_library (
    lib-asio STATIC
    test/lib_asio.cpp
)

target_link_libraries(lib-asio PUBLIC Threads::Threads)

set_property(TARGET lib-asio PROPERTY FOLDER "static-libs")

add_library (
    lib-asio-ssl STATIC
    test/lib_asio_ssl.cpp
)

target_link_libraries(lib-asio-ssl PUBLIC Threads::Threads)

set_property(TARGET lib-asio-ssl PROPERTY FOLDER "static-libs")

add_library (
    lib-beast STATIC
    test/lib_beast.cpp
)

target_link_libraries(lib-beast PUBLIC Threads::Threads)

set_property(TARGET lib-beast PROPERTY FOLDER "static-libs")

add_library (
    lib-beast-extra STATIC
    test/lib_beast_extra.cpp
)

set_property(TARGET lib-beast-extra PROPERTY FOLDER "static-libs")

target_link_libraries(lib-beast-extra PUBLIC Threads::Threads)

add_library (
    lib-test STATIC
    test/lib_test.cpp
)

target_link_libraries(lib-test PUBLIC Threads::Threads)

set_property(TARGET lib-test PROPERTY FOLDER "static-libs")

include_directories (.)

if (OPENSSL_FOUND)
    include_directories (${OPENSSL_INCLUDE_DIR})
endif()

include_directories (${PROJECT_SOURCE_DIR}/include)

add_subdirectory (server)
add_subdirectory (test)

