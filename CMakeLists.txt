PROJECT(Pheet)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
find_package(Threads REQUIRED)

include(FindPkgConfig)
pkg_search_module(HWLOC REQUIRED hwloc)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c++11 -fmessage-length=0 -fno-omit-frame-pointer")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DPHEET_DEBUG_MODE)
endif()

add_subdirectory(test)

# A silly hack to make all files (including headers) show up in QtCreator.
FILE(GLOB_RECURSE ALL_HEADERS *.h)
add_library(qtcreator_dummy ${ALL_HEADERS} test/Test.cpp)
