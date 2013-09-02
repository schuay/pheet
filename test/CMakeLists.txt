find_library(LIBCGRAPH_LIBRARY cgraph REQUIRED)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable (pheet_test ${ALL_CPP_SRCS})
target_link_libraries(pheet_test
    ${BLAS_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${LIBCGRAPH_LIBRARY}
)
