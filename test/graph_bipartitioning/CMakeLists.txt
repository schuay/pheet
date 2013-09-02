FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable(graph_bipartitioning ${ALL_CPP_SRCS})
target_link_libraries(graph_bipartitioning
    ${BLAS_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LAPACK_LIBRARIES}
)
