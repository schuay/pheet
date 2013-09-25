add_executable(msp-bench
    main.cpp
    MspBenchmarks.cpp
    ../../Test.cpp
)
target_link_libraries(msp-bench
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    ${LIBCGRAPH_LIBRARY}
    msp-generator
    msp-graph
    msp-pareto
    msp-sequential
    msp-sp
)

add_executable(msp-bench-gengraph gengraph.cpp)
target_link_libraries(msp-bench-gengraph
    ${LIBCGRAPH_LIBRARY}
    msp-generator
    msp-graph
)
