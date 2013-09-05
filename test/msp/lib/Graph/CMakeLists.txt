add_library(msp-graph
    Edge.cpp
    Graph.cpp
    Node.cpp
)

target_link_libraries(msp-graph
    ${LIBCGRAPH_LIBRARY}
)

add_subdirectory(Adapters)
add_subdirectory(Generator)
