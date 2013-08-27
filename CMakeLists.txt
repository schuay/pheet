PROJECT(Pheet)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

find_package(BLAS)
find_package(LAPACK)
find_package(Threads)

include(FindPkgConfig)
pkg_search_module(HWLOC REQUIRED hwloc)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions("-Wall -Wextra -pedantic -std=c++11 -fmessage-length=0 -fno-omit-frame-pointer")
endif()

FILE(GLOB_RECURSE ALL_CPP_SRCS test/*.cpp)
FILE(GLOB_RECURSE ALL_HEADERS *.h)
FILE(GLOB_RECURSE Nonworking_Tests test/inarow/*.cpp test/uts/*.cpp  test/sor/*.cpp)
#LIST(REMOVE_ITEM ALL_CPP_SRCS  ${Nonworking_Tests} )

LINK_DIRECTORIES(/opt/gcc-4.7/lib/ /opt/gcc-4.7/lib64/ /opt/hwloc-1.4/lib/)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable (pheet_test ${ALL_CPP_SRCS} ${ALL_HEADERS})
target_link_libraries(pheet_test
    ${BLAS_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LAPACK_LIBRARIES}
)
SET_TARGET_PROPERTIES(pheet_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin")
