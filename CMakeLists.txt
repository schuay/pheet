PROJECT(Pheet)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FILE(GLOB_RECURSE ALL_CPP_SRCS test/*.cpp)
FILE(GLOB_RECURSE ALL_HEADERS *.h)
FILE(GLOB_RECURSE Nonworking_Tests test/inarow/*.cpp test/uts/*.cpp  test/sor/*.cpp)
#LIST(REMOVE_ITEM ALL_CPP_SRCS  ${Nonworking_Tests} )

LINK_DIRECTORIES(/opt/gcc-4.7/lib/ /opt/gcc-4.7/lib64/ /opt/hwloc-1.4/lib/)

include_directories( . /opt/hwloc-1.4/include/)
add_executable (pheet_test ${ALL_CPP_SRCS} ${ALL_HEADERS})
target_link_libraries(pheet_test hwloc pthread blas lapack)
set(CMAKE_CXX_FLAGS "-std=c++0x -Wall -fmessage-length=0 -fno-omit-frame-pointer")
SET_TARGET_PROPERTIES(pheet_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin")
