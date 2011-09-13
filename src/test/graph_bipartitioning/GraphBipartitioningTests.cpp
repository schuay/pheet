/*
 * GraphBipartitioningTests.cpp
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "GraphBipartitioningTests.h"
#include "BranchBound/BranchBoundGraphBipartitioning.h"
#include "BranchBound/BasicLowerBound.h"
#include "BranchBound/BasicNextVertex.h"

#include "../test_schedulers.h"

namespace pheet {

GraphBipartitioningTests::GraphBipartitioningTests() {

}

GraphBipartitioningTests::~GraphBipartitioningTests() {

}

void GraphBipartitioningTests::run_test() {
	if(graph_bipartitioning_test) {
		std::cout << "----" << std::endl;

	//	this->run_partitioner<BranchBoundGraphBipartitioning<DefaultMixedModeScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<DefaultBasicScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<DefaultSynchroneousScheduler, BasicLowerBound, BasicNextVertex> >();
	}
}

}
