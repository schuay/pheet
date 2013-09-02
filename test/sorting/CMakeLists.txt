FILE(GLOB_RECURSE ALL_CPP_SRCS *.cpp)

include_directories(${HWLOC_INCLUDE_DIR} ${CMAKE_SOURCE_DIR})
add_executable(sorting ${ALL_CPP_SRCS} ../Test.cpp)
target_link_libraries(sorting
    ${CMAKE_THREAD_LIBS_INIT}
    ${HWLOC_LIBRARIES}
)
