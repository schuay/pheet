add_library(msp-sp
    Path.cpp
    ShortestPaths.cpp
)

target_link_libraries(msp-sp
    msp-graph
)
