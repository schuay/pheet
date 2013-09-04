include_directories(${GTEST_INCLUDE_DIR})
add_executable(msp-test MspTest.cpp)
target_link_libraries(msp-test
    ${CMAKE_THREAD_LIBS_INIT}
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
    ${HWLOC_LIBRARIES}
    msp-generator
    msp-graph
    msp-pareto
    msp-sequential
    msp-sp
)
add_test(NAME msp-test COMMAND msp-test)
