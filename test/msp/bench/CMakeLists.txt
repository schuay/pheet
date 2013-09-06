FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

add_executable(msp-bench ${ALL_CPP_SRCS} ../../Test.cpp)
target_link_libraries(msp-bench
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LIBCGRAPH_LIBRARY}
    msp-graph
    msp-pareto
    msp-sequential
    msp-sp
)
