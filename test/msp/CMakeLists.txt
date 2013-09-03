find_library(LIBCGRAPH_LIBRARY cgraph REQUIRED)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}
	${HWLOC_INCLUDE_DIR}
)

add_subdirectory(Sequential)
add_subdirectory(bench)
add_subdirectory(lib)
add_subdirectory(test)
