FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable(count_bench ${ALL_CPP_SRCS})
target_link_libraries(count_bench
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
)
