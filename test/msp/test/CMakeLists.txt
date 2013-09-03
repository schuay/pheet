include_directories(${GTEST_INCLUDE_DIR})
add_executable(msp-test-seq SequentialMsp.cpp)
target_link_libraries(msp-test-seq
	${GTEST_LIBRARY}
	${GTEST_MAIN_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	msp-generator
	msp-graph
        msp-pareto
        msp-sequential
        msp-sp
)
add_test(NAME msp-test-seq COMMAND msp-test-seq)
