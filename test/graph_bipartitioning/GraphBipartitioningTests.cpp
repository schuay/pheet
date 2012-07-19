/*
 * GraphBipartitioningTests.cpp
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#include "GraphBipartitioningTests.h"

#ifdef GRAPH_BIPARTITIONING_TEST
/*
#include "BranchBound/BranchBoundGraphBipartitioning.h"
#include "BranchBound/BasicLowerBound.h"
#include "BranchBound/BasicNextVertex.h"
#include "StrategyBranchBound/StrategyBranchBoundGraphBipartitioning.h"
#include "StrategyBranchBound/BranchBoundGraphBipartitioningAutoStrategy.h"
#include "StrategyBranchBound/BranchBoundGraphBipartitioningBestFirstStrategy.h"

#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningBasicLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningImprovedLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaNVLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaContribNVLogic.h"
*/
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic.h"

#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/sched/Centralized/CentralizedScheduler.h>
#include <pheet/sched/Strategy/StrategyScheduler.h>

#include <pheet/ds/Queue/GlobalLock/GlobalLockQueue.h>
#include <pheet/ds/MultiSet/GlobalLock/GlobalLockMultiSet.h>

#include <pheet/primitives/Mutex/TASLock/TASLock.h>
#include <pheet/primitives/Mutex/TTASLock/TTASLock.h>

#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioning.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioning.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy.h"

#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioning.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy.h"
#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy.h"



//#include "PPoPPImprovedStrategyBranchBound/PPoPPImprovedStrategyBranchBoundGraphBipartitioning.h"


/*
#include "../../sched/strategies/Fifo/FifoStrategy.h"
#include "../../sched/strategies/Lifo/LifoStrategy.h"
#include "../../sched/strategies/LifoFifo/LifoFifoStrategy.h"

#include "../test_schedulers.h"*/
#endif

namespace pheet {

GraphBipartitioningTests::GraphBipartitioningTests() {

}

GraphBipartitioningTests::~GraphBipartitioningTests() {

}

void GraphBipartitioningTests::run_test() {
#ifdef GRAPH_BIPARTITIONING_TEST
	std::cout << "----" << std::endl;

#ifdef AMP_STEALING_DEQUE_TEST
//	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>::WithTaskStorage<YourImplementation>,
//							ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>,
							ImprovedBranchBoundGraphBipartitioning>();

#elif AMP_QUEUE_STACK_TEST

	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();

	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>::WithTaskStorage<GlobalLockQueue>,
						ImprovedBranchBoundGraphBipartitioning>();

	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();

#elif AMP_SKIPLIST_TEST
	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>::WithPriorityTaskStorage<GlobalLockMultiSetPriorityQueue>,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();


#elif AMP_LOCK_TEST

	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>::WithMutex<TASLock>,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<CentralizedScheduler>::WithMutex<TTASLock>,
						ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>,
						ImprovedBranchBoundGraphBipartitioning>();

#else
/*
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning>(); // strategy: ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy>::T >();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet,
							ImprovedStrategyBranchBoundGraphBipartitioning>();
	this->run_partitioner<	Pheet,
							ImprovedBranchBoundGraphBipartitioning>();*/
	this->run_partitioner<	Pheet::WithScheduler<BasicScheduler>,
							ImprovedBranchBoundGraphBipartitioning>();
	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy>::T >();
	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy>::T >();
	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>
								::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy>
								::WithLogic<ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic>
								::T >();
//	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
//							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy>::T >();

	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy>::T >();
	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
	PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>
									::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy>
									::WithLogic<ImprovedBranchBoundGraphBipartitioningDeltaContribNVFREELogic>
									::T >();

	/*	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning>();

	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy>::T >();
  	this->run_partitioner<  Pheet::WithScheduler<StrategyScheduler>,
  							PPoPPImprovedStrategyBranchBoundGraphBipartitioning<>::WithSchedulingStrategy<PPoPPImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy>::T >();
  							*/

#endif

#endif
}

}

