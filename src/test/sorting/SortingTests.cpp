/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SortingTests.h"

#include "../../sched/Basic/Synchroneous/SynchroneousScheduler.h"
#include "../../sched/Basic/Basic/BasicScheduler.h"
#include "../../sched/MixedMode/Basic/BasicMixedModeScheduler.h"

#include "Reference/ReferenceSTLSort.h"
#include "Reference/ReferenceQuicksort.h"
#include "Dag/DagQuicksort.h"
#include "MixedMode/MixedModeQuicksort.h"

#include "../../em/CPUHierarchy/Oversubscribed/OversubscribedSimpleCPUHierarchy.h"

#include "../../ds/CircularArray/FixedSize/FixedSizeCircularArray.h"
#include "../../ds/CircularArray/TwoLevelGrowing/TwoLevelGrowingCircularArray.h"
#include "../../ds/StealingDeque/CircularArray/CircularArrayStealingDeque.h"

#include "../../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../../primitives/Barrier/Simple/SimpleBarrier.h"

#include <iostream>

namespace pheet {

SortingTests::SortingTests() {

}

SortingTests::~SortingTests() {

}

//using FixedSizeCircularArrayStealingDeque = CircularArrayStealingDeque<T, FixedSizeCircularArray<T> >;
template <typename T>
class FixedSizeCircularArrayStealingDeque : public CircularArrayStealingDeque<T, FixedSizeCircularArray > {
public:
	FixedSizeCircularArrayStealingDeque(size_t initial_capacity);
	FixedSizeCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<stealing_deque_count_pop_cas>& num_pop_cas);
};

template <typename T>
FixedSizeCircularArrayStealingDeque<T>::FixedSizeCircularArrayStealingDeque(size_t initial_capacity)
: CircularArrayStealingDeque<T, FixedSizeCircularArray >(initial_capacity){

}

template <typename T>
FixedSizeCircularArrayStealingDeque<T>::FixedSizeCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<stealing_deque_count_pop_cas>& num_pop_cas)
: CircularArrayStealingDeque<T, FixedSizeCircularArray >(initial_capacity, num_stolen, num_pop_cas){

}

template <typename T>
class MyTwoLevelGrowingCircularArray : public TwoLevelGrowingCircularArray<T> {
public:
	MyTwoLevelGrowingCircularArray(size_t initial_capacity);
};

template <typename T>
MyTwoLevelGrowingCircularArray<T>::MyTwoLevelGrowingCircularArray(size_t initial_capacity)
: TwoLevelGrowingCircularArray<T>(initial_capacity){

}

template <typename T>
class TwoLevelGrowingCircularArrayStealingDeque : public CircularArrayStealingDeque<T, MyTwoLevelGrowingCircularArray > {
public:
	TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity);
	TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<stealing_deque_count_pop_cas>& num_pop_cas);
};

template <typename T>
TwoLevelGrowingCircularArrayStealingDeque<T>::TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity)
: CircularArrayStealingDeque<T, MyTwoLevelGrowingCircularArray >(initial_capacity){

}

template <typename T>
TwoLevelGrowingCircularArrayStealingDeque<T>::TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<stealing_deque_count_pop_cas>& num_pop_cas)
: CircularArrayStealingDeque<T, MyTwoLevelGrowingCircularArray >(initial_capacity, num_stolen, num_pop_cas){

}

void SortingTests::run_test() {
	std::cout << "----" << std::endl;
	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttime\truns" << std::endl;

	this->run_sorter<MixedModeQuicksort<BasicMixedModeScheduler<OversubscribedSimpleCPUHierarchy, TwoLevelGrowingCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff> > >();
	this->run_sorter<DagQuicksort<BasicMixedModeScheduler<OversubscribedSimpleCPUHierarchy, TwoLevelGrowingCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff> > >();
	this->run_sorter<DagQuicksort<BasicScheduler<OversubscribedSimpleCPUHierarchy, FixedSizeCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff> > >();
	this->run_sorter<DagQuicksort<SynchroneousScheduler<OversubscribedSimpleCPUHierarchy> > >();
	this->run_sorter<ReferenceQuicksort>();
	this->run_sorter<ReferenceSTLSort>();
}

}
