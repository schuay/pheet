add_library(msp-graph-adapters
    PaixaoSantos.cpp
    Sssp.cpp
)

add_executable(convps
    convps.cpp
)
target_link_libraries(convps
    ${LIBCGRAPH_LIBRARY}
    msp-graph
    msp-graph-adapters
)
