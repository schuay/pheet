add_subdirectory(/usr/src/gtest gtest)

include_directories(${GTEST_INCLUDE_DIR})
add_executable(msp-test MspTest.cpp)
target_link_libraries(msp-test
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    gtest
    msp-generator
    msp-graph
    msp-pareto
    msp-sequential
    msp-sp
)
add_test(NAME msp-test COMMAND msp-test)
