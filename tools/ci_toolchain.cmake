mark_as_advanced(FORCE CMAKE_TOOLCHAIN_FILE)

if (BEAST_LOUNGE_CONFIGURED)
    return()
endif()

if (DEFINED ENV{BEAST_LOUNGE_SANITIZE})
    set(CMAKE_CXX_FLAGS_INIT
        "${CMAKE_CXX_FLAGS_INIT} -fsanitize=$ENV{BEAST_LOUNGE_SANITIZE} -fno-sanitize-recover=$ENV{BEAST_LOUNGE_SANITIZE}")
    message(STATUS "Enabling sanitizers: $ENV{BEAST_LOUNGE_SANITIZE}")
endif()

if (DEFINED ENV{BEAST_LOUNGE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE
        $ENV{BEAST_LOUNGE_BUILD_TYPE})
    message(STATUS "Build type: $ENV{BEAST_LOUNGE_BUILD_TYPE}")
endif()

if (DEFINED ENV{BEAST_LOUNGE_COVERAGE})
    set(CMAKE_CXX_FLAGS_INIT
        "${CMAKE_CXX_FLAGS_INIT} -fprofile-arcs --coverage")
    message(STATUS "Enabling coverage collection ${CMAKE_CXX_FLAGS}")
endif()

set(BEAST_LOUNGE_CONFIGURED TRUE)
