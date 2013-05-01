/*
 * PrefixSumTests.cpp
 *
 *  Created on: Jun 28, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#include "PrefixSumTests.h"

#ifdef PREFIX_SUM_TEST
#include "Sequential/SequentialPrefixSum.h"
#include "NaiveParallel/NaiveParallelPrefixSum.h"
#include "RecursiveParallel/RecursiveParallelPrefixSum.h"
#include "StrategyRecursiveParallel/StrategyRecursiveParallelPrefixSum.h"
#include "Parallel/ParallelPrefixSum.h"
#include "Strategy/StrategyPrefixSum.h"

//#include <pheet/ds/StealingDeque/CircularArray11/CircularArrayStealingDeque11.h>
#include <pheet/ds/PriorityQueue/Heap/Heap.h>
#include <pheet/ds/PriorityQueue/STLPriorityQueueWrapper/STLPriorityQueueWrapper.h>
//#include <pheet/ds/PriorityQueue/SortedArrayHeap/SortedArrayHeap.h>
#include <pheet/ds/PriorityQueue/Fibonacci/FibonacciHeap.h>
#include <pheet/ds/PriorityQueue/FibonacciSame/FibonacciSameHeap.h>
#include <pheet/ds/PriorityQueue/Fibolike/FibolikeHeap.h>
#include <pheet/ds/Queue/GlobalLock/GlobalLockQueue.h>
#include <pheet/ds/MultiSet/GlobalLock/GlobalLockMultiSet.h>
#include <pheet/ds/StrategyHeap/Volatile/VolatileStrategyHeap.h>
#include <pheet/ds/StrategyTaskStorage/DistK/DistKStrategyTaskStorage.h>

#include <pheet/primitives/Mutex/TASLock/TASLock.h>
#include <pheet/primitives/Mutex/TTASLock/TTASLock.h>

#include <pheet/pheet.h>
#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/sched/Finisher/FinisherScheduler.h>
#include <pheet/sched/Centralized/CentralizedScheduler.h>
#include <pheet/sched/CentralizedPriority/CentralizedPriorityScheduler.h>
#include <pheet/sched/Strategy/StrategyScheduler.h>
#include <pheet/sched/BStrategy/BStrategyScheduler.h>
#include <pheet/sched/Synchroneous/SynchroneousScheduler.h>
#include <pheet/sched/MixedMode/MixedModeScheduler.h>


#include <iostream>
#endif

namespace pheet {


PrefixSumTests::PrefixSumTests() {

}

PrefixSumTests::~PrefixSumTests() {

}

void PrefixSumTests::run_test() {
#ifdef PREFIX_SUM_TEST
	std::cout << "----" << std::endl;

	// default tests
	this->run_prefix_sum<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorage>,
							StrategyRecursiveParallelPrefixSum>();
	this->run_prefix_sum<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorageLocalK>,
							StrategyRecursiveParallelPrefixSum>();
	this->run_prefix_sum<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<CentralKStrategyTaskStorage>,
							StrategyRecursiveParallelPrefixSum>();
	this->run_prefix_sum<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<CentralKStrategyTaskStorageLocalK>,
							StrategyRecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<StrategyScheduler>,
						StrategyRecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						StrategyRecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<SynchroneousScheduler>,
						StrategyRecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<StrategyScheduler>,
						RecursiveParallelPrefixSum>();
/*
	this->run_prefix_sum<	Pheet,
						RecursiveParallelPrefixSum>();*/

	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						RecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<SynchroneousScheduler>,
						RecursiveParallelPrefixSum>();

/*
	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						RecursiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<SynchroneousScheduler>,
						SequentialPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						NaiveParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						ParallelPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<BasicScheduler>,
						StrategyPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<StrategyScheduler>,
						StrategyPrefixSum>();

	this->run_prefix_sum<	Pheet::WithScheduler<SynchroneousScheduler>,
						StrategyPrefixSum>();*/

#endif
}


}
