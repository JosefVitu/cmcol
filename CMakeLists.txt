cmake_minimum_required (VERSION 3.0.0)
project (cmcol)

include(CheckCXXCompilerFlag)
include(FindPkgConfig)

pkg_check_modules(LIBXML libxml-2.0)
pkg_check_modules(TAGLIB REQUIRED taglib>=1.9)

if (LIBXML_FOUND)
    link_libraries (${LIBXML_LIBRARIES})
    include_directories(${LIBXML_INCLUDE_DIRS})
endif (LIBXML_FOUND)

if (TAGLIB_FOUND)
    link_libraries (${TAGLIB_LIBRARIES})
    include_directories(${TAGLIB_INCLUDE_DIRS})
endif (TAGLIB_FOUND)

if (NOT CMAKE_BUILD_TYPE)
       set (CMAKE_BUILD_TYPE Release)
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
    else ()
        set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    ## Clang or AppleClang
    #set(CMAKE_COMPILER_IS_CLANGXX 1)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Weverything -std=c++11")
    else ()
        set(CMAKE_CXX_FLAGS "-O3 -Wall -std=c++11")
    endif ()
endif ()

## Strip
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_EXE_LINKER_FLAGS "-s")
endif ()

add_executable (cmcol main.cpp)

install (TARGETS cmcol DESTINATION bin)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
