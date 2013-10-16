add_library(msp-sequential SequentialMsp.cpp)
target_link_libraries(msp-sequential
	msp-graph
	msp-pareto
)
