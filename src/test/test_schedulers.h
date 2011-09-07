/*
 * test_schedulers.h
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef TEST_SCHEDULERS_H_
#define TEST_SCHEDULERS_H_

#include "../../sched/Basic/Synchroneous/SynchroneousScheduler.h"
#include "../../sched/Basic/Basic/BasicScheduler.h"
#include "../../sched/MixedMode/Basic/BasicMixedModeScheduler.h"

#include "../../em/CPUHierarchy/Oversubscribed/OversubscribedSimpleCPUHierarchy.h"

#include "../../ds/CircularArray/FixedSize/FixedSizeCircularArray.h"
#include "../../ds/CircularArray/TwoLevelGrowing/TwoLevelGrowingCircularArray.h"
#include "../../ds/StealingDeque/CircularArray/CircularArrayStealingDeque.h"

#include "../../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../../primitives/Barrier/Simple/SimpleBarrier.h"

namespace pheet {

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

typedef BasicMixedModeScheduler<OversubscribedSimpleCPUHierarchy, TwoLevelGrowingCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff>
	DefaultMixedModeScheduler;
typedef BasicScheduler<OversubscribedSimpleCPUHierarchy, FixedSizeCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff>
	DefaultBasicScheduler;
typedef SynchroneousScheduler<OversubscribedSimpleCPUHierarchy>
	DefaultSynchroneousScheduler;

}

#endif /* TEST_SCHEDULERS_H_ */
