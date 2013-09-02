find_library(LIBCGRAPH_LIBRARY cgraph REQUIRED)

FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable(msp ${ALL_CPP_SRCS})
target_link_libraries(msp
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LIBCGRAPH_LIBRARY}
)
