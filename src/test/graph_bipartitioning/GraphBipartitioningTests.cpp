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
#include "StrategyBranchBound/StrategyBranchBoundGraphBipartitioning.h"
#include "StrategyBranchBound/BranchBoundGraphBipartitioningAutoStrategy.h"
#include "StrategyBranchBound/BranchBoundGraphBipartitioningBestFirstStrategy.h"
#include "../../sched/strategies/Fifo/FifoStrategy.h"
#include "../../sched/strategies/Lifo/LifoStrategy.h"
#include "../../sched/strategies/LifoFifo/LifoFifoStrategy.h"

#include "../test_schedulers.h"

namespace pheet {

GraphBipartitioningTests::GraphBipartitioningTests() {

}

GraphBipartitioningTests::~GraphBipartitioningTests() {

}

void GraphBipartitioningTests::run_test() {
	if(graph_bipartitioning_test) {
		std::cout << "----" << std::endl;

		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();

		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		// Too slow...
		// this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerVeryLongQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPrioritySchedulerShortQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPrioritySchedulerLongQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPrioritySchedulerVeryLongQueues, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningBestFirstStrategy > >();

		this->run_partitioner<BranchBoundGraphBipartitioning<PrimitivePriorityScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<FallbackPriorityScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<DefaultMixedModeScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<DefaultBasicScheduler, BasicLowerBound, BasicNextVertex> >();
		this->run_partitioner<BranchBoundGraphBipartitioning<DefaultSynchroneousScheduler, BasicLowerBound, BasicNextVertex> >();

		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoFifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoFifoStrategy> > >();
	}
}

}
