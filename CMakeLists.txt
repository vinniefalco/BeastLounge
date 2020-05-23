#
# Copyright (c) 2018-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/vinniefalco/BeastLounge
#

cmake_minimum_required(VERSION 3.5...3.16)

set(BEAST_LOUNGE_VERSION 1)
project(beast_lounge VERSION "${BEAST_LOUNGE_VERSION}" LANGUAGES CXX)
set_property (GLOBAL PROPERTY USE_FOLDERS ON)
set_property (GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER build)

#-------------------------------------------------------------------------------

find_package (Threads)

set(Boost_USE_STATIC_RUNTIME On)
find_package (Boost 1.73 REQUIRED COMPONENTS json system)

#-------------------------------------------------------------------------------

file(GLOB_RECURSE BEAST_LOUNGE_HEADERS
    add_compile_definitions$<$<VERSION_GREATER_EQUAL:${CMAKE_VERSION},3.12>:CONFIGURE_DEPENDS>
    include/lounge/*.hpp
)

file(GLOB_RECURSE BEAST_LOUNGE_SOURCES
    add_compile_definitions$<$<VERSION_GREATER_EQUAL:${CMAKE_VERSION},3.12>:CONFIGURE_DEPENDS>
    src/*.hpp
    src/*.cpp
)

source_group(TREE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/lounge PREFIX "lounge" FILES ${BEAST_LOUNGE_HEADERS})
source_group(TREE
    ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${BEAST_LOUNGE_SOURCES})

add_library(lounge ${BEAST_LOUNGE_HEADERS} ${BEAST_LOUNGE_SOURCES})

target_compile_features(lounge PUBLIC cxx_constexpr)

include(GNUInstallDirs)
target_include_directories(lounge
    PRIVATE
        src
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_compile_definitions(lounge PUBLIC
    )

target_link_libraries(lounge
    PUBLIC
        #Boost::json
        Boost::system
)

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

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

set (Boost_USE_STATIC_LIBS ON)

if (MSVC)
  #set (Boost_USE_STATIC_RUNTIME ON)
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
    set( CMAKE_CXX_FLAGS
      "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unused-parameter")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wrange-loop-analysis")
    endif ()

endif()

find_package (Threads)

#-------------------------------------------------------------------------------
#
# Boost / Beast
#
#-------------------------------------------------------------------------------

#find_package (Boost 1.73 CONFIG REQUIRED json system thread)
find_package (Boost 1.73 CONFIG REQUIRED system thread)
include_directories (${Boost_INCLUDE_DIRS})
link_directories (${Boost_LIBRARY_DIRS})

add_definitions (-DBOOST_ASIO_NO_DEPRECATED=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_ARRAY=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_BIND=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_DATE_TIME=1)
add_definitions (-DBOOST_ASIO_DISABLE_BOOST_REGEX=1)
add_definitions (-DBOOST_ASIO_SEPARATE_COMPILATION=1)
add_definitions (-DBOOST_BEAST_SEPARATE_COMPILATION=1)

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

target_link_libraries (lib-asio PUBLIC Threads::Threads)

set_property (TARGET lib-asio PROPERTY FOLDER "static-libs")

#-------------------------------------------------------------------------------

add_library (
    lib-asio-ssl STATIC
    test/lib_asio_ssl.cpp
)

target_link_libraries (lib-asio-ssl PUBLIC Threads::Threads lib-asio)

set_property (TARGET lib-asio-ssl PROPERTY FOLDER "static-libs")

#-------------------------------------------------------------------------------

add_library (
    lib-beast STATIC
    test/lib_beast.cpp
)

target_link_libraries(lib-beast PUBLIC Threads::Threads lib-asio)

set_property (TARGET lib-beast PROPERTY FOLDER "static-libs")

#-------------------------------------------------------------------------------

add_library (
    lib-json STATIC
    test/lib_json.cpp
)

target_link_libraries (lib-json PUBLIC Threads::Threads)

set_property (TARGET lib-json PROPERTY FOLDER "static-libs")

#-------------------------------------------------------------------------------

add_library (
    lib-test STATIC
    test/lib_test.cpp
)

target_link_libraries (lib-test PUBLIC Threads::Threads)

set_property (TARGET lib-test PROPERTY FOLDER "static-libs")

#-------------------------------------------------------------------------------

include_directories (.)

if (OPENSSL_FOUND)
    include_directories (${OPENSSL_INCLUDE_DIR})
endif()

include_directories (${PROJECT_SOURCE_DIR}/include)

include(CTest)

add_subdirectory (app)
add_subdirectory (server)
add_subdirectory (static)
if (BUILD_TESTING)
    add_subdirectory (test)
endif()
