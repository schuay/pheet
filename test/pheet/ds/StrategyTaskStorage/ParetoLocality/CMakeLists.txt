include_directories(
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
)

add_executable(virtual-array-test VirtualArrayTest.cpp)
target_link_libraries(virtual-array-test
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
    gtest
)
add_test(NAME virtual-array-test COMMAND virtual-array-test)
