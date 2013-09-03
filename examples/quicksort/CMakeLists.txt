include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})

add_executable(parallel parallel.cpp)
target_link_libraries(parallel ${CMAKE_THREAD_LIBS_INIT} ${HWLOC_LIBRARIES})

add_executable(sequential sequential.cpp)
target_link_libraries(sequential ${CMAKE_THREAD_LIBS_INIT} ${HWLOC_LIBRARIES})
