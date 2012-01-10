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
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioning.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningBasicLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningImprovedLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaLogic.h"
#include "ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioningDeltaNVLogic.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioning.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h"
#include "ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy.h"
#include "../../sched/strategies/Fifo/FifoStrategy.h"
#include "../../sched/strategies/Lifo/LifoStrategy.h"
#include "../../sched/strategies/LifoFifo/LifoFifoStrategy.h"

#include "../test_schedulers.h"

namespace pheet {


template <class Scheduler, typename T>
class AutoLifoStrategy : public ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, LifoStrategy, T> {
public:
	template <typename ... ConsParams>
	AutoLifoStrategy(ConsParams&& ... params)
	: ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, LifoStrategy, T >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class AutoFifoStrategy : public ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, FifoStrategy, T> {
public:
	template <typename ... ConsParams>
	AutoFifoStrategy(ConsParams&& ... params)
	: ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, FifoStrategy, T >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class AutoLifoFifoStrategy : public ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, LifoFifoStrategy, T> {
public:
	template <typename ... ConsParams>
	AutoLifoFifoStrategy(ConsParams&& ... params)
	: ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy<Scheduler, LifoFifoStrategy, T >(static_cast<ConsParams&&>(params) ...) {}
};

GraphBipartitioningTests::GraphBipartitioningTests() {

}

GraphBipartitioningTests::~GraphBipartitioningTests() {

}

void GraphBipartitioningTests::run_test() {
	if(graph_bipartitioning_test) {
		std::cout << "----" << std::endl;

		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapMultiStealPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapMultiStealPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<ArrayListHeapMultiStealPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapMultiStealPriorityScheduler, 64>, 64 > >();
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<DefaultBasicScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<DefaultBasicScheduler, 64>, 64 > >();

		/*
		// 1. All data-structures, with prio, DeltaNV logic, all strategies, all queue lengths except very long (very long only for single case)

		// ArrayListHeap, prio, DeltaNV, BestFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, DepthFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LowerBound, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, FIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, AutoFifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, AutoLifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFOFIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerShortQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// ArrayListHeap, prio, DeltaNV, BestFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, DepthFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LowerBound, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, FIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, AutoFifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, AutoLifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFOFIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPriorityScheduler, 64>, AutoLifoFifoStrategy, 64 > >();

		// ArrayListHeap, prio, DeltaNV, BestFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, DepthFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LowerBound, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, FIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoFifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoLifoStrategy, 64 > >();
		// ArrayListHeap, prio, DeltaNV, LIFOFIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// ArrayListHeap, prio, DeltaNV, BestFirst, 16
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerVeryLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListHeapPrioritySchedulerVeryLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();

		// ArrayList, prio, DeltaNV, BestFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, DepthFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LowerBound, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, FIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, AutoFifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, AutoLifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFOFIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerShortQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// ArrayList, prio, DeltaNV, BestFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, DepthFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LowerBound, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, FIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoFifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFOFIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoFifoStrategy, 64 > >();

		// ArrayList, prio, DeltaNV, BestFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, DepthFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LowerBound, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, FIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, AutoFifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, AutoLifoStrategy, 64 > >();
		// ArrayList, prio, DeltaNV, LIFOFIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPrioritySchedulerLongQueues, 64>, AutoLifoFifoStrategy, 64 > >();


		// PrimitivePheetHeap, prio, DeltaNV, BestFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, DepthFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LowerBound, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, FIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, AutoFifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, AutoLifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFOFIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerShortQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// PrimitivePheetHeap, prio, DeltaNV, BestFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, DepthFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LowerBound, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, FIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoFifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFOFIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoFifoStrategy, 64 > >();

		// PrimitivePheetHeap, prio, DeltaNV, BestFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, DepthFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LowerBound, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, FIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, AutoFifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, AutoLifoStrategy, 64 > >();
		// PrimitivePheetHeap, prio, DeltaNV, LIFOFIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePheetHeapPrioritySchedulerLongQueues, 64>, AutoLifoFifoStrategy, 64 > >();


		// Primitive, prio, DeltaNV, BestFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, DepthFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LowerBound, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// Primitive, prio, DeltaNV, FIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, AutoFifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, AutoLifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFOFIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerShortQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerShortQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// Primitive, prio, DeltaNV, BestFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, DepthFirst, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LowerBound, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// Primitive, prio, DeltaNV, FIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoFifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFOFIFO, 4
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<ArrayListPriorityScheduler, 64>, AutoLifoFifoStrategy, 64 > >();

		// Primitive, prio, DeltaNV, BestFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, DepthFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LowerBound, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// Primitive, prio, DeltaNV, FIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, AutoFifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, AutoLifoStrategy, 64 > >();
		// Primitive, prio, DeltaNV, LIFOFIFO, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<PrimitivePrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<PrimitivePrioritySchedulerLongQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// 2. ArrayListHeap, with prio, Delta logic, all strategies, 8

		// ArrayListHeap, prio, Delta, BestFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();
		// ArrayListHeap, prio, Delta, DepthFirst, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy, 64 > >();
		// ArrayListHeap, prio, Delta, LowerBound, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy, 64 > >();
		// ArrayListHeap, prio, Delta, FIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoFifoStrategy, 64 > >();
		// ArrayListHeap, prio, Delta, LIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoLifoStrategy, 64 > >();
		// ArrayListHeap, prio, Delta, LIFOFIFO, 1
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningDeltaLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, AutoLifoFifoStrategy, 64 > >();

		// 3. ArrayListHeap, with prio, improved logic, BestFirst strategy, 8

		// ArrayListHeap, prio, Improved, BestFirst, 8
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListHeapPrioritySchedulerLongQueues, ImprovedBranchBoundGraphBipartitioningImprovedLogic<ArrayListHeapPrioritySchedulerLongQueues, 64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();

		// 4. -, basic sched, all logics, -, .

		// -, basic, DeltaNV, -, -
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<DefaultBasicScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<DefaultBasicScheduler, 64>, 64 > >();
		// -, basic, Delta, -, -
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<DefaultBasicScheduler, ImprovedBranchBoundGraphBipartitioningDeltaLogic<DefaultBasicScheduler, 64>, 64 > >();
		// -, basic, Improved, -, -
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<DefaultBasicScheduler, ImprovedBranchBoundGraphBipartitioningImprovedLogic<DefaultBasicScheduler, 64>, 64 > >();

		// 5. -, synch sched, DeltaNV logic, -, .

		// -, synch, DeltaNV, -, -
		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<DefaultSynchroneousScheduler, ImprovedBranchBoundGraphBipartitioningDeltaNVLogic<DefaultSynchroneousScheduler, 64>, 64 > >();
*/
		// All to slow compared to newer implementations
/*		this->run_partitioner<ImprovedBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningBasicLogic<64>, 64 > >();
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningBasicLogic<64>, AutoFifoStrategy, 64 > >();
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningBasicLogic<64>, AutoLifoStrategy, 64 > >();
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningBasicLogic<64>, AutoLifoFifoStrategy, 64 > >();
		this->run_partitioner<ImprovedStrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, ImprovedBranchBoundGraphBipartitioningBasicLogic<64>, ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy, 64 > >();

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
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitivePheetHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveSortedArrayHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<FifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<PrimitiveHeapPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoFifoStrategy> > >();
		this->run_partitioner<StrategyBranchBoundGraphBipartitioning<ArrayListPriorityScheduler, BasicLowerBound, BasicNextVertex, BranchBoundGraphBipartitioningAutoStrategy<LifoFifoStrategy> > >();
		*/
	}
}

}
