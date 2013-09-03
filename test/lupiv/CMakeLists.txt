FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable(lupiv ${ALL_CPP_SRCS} ../Test.cpp)
target_link_libraries(lupiv
    ${BLAS_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LAPACK_LIBRARIES}
)
