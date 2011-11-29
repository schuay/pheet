/*
 * test_schedulers.h
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef TEST_SCHEDULERS_H_
#define TEST_SCHEDULERS_H_

#include "../sched/Basic/Synchroneous/SynchroneousScheduler.h"
#include "../sched/Basic/Basic/BasicScheduler.h"
#include "../sched/Basic/Priority/PriorityScheduler.h"
#include "../sched/MixedMode/Basic/BasicMixedModeScheduler.h"

#include "../sched/strategies/LifoFifo/LifoFifoStrategy.h"
#include "../sched/strategies/UserDefinedPriority/UserDefinedPriority.h"


#include "../em/CPUHierarchy/Oversubscribed/OversubscribedSimpleCPUHierarchy.h"

#include "../ds/CircularArray/FixedSize/FixedSizeCircularArray.h"
#include "../ds/CircularArray/TwoLevelGrowing/TwoLevelGrowingCircularArray.h"
#include "../ds/StealingDeque/CircularArray/CircularArrayStealingDeque.h"
#include "../ds/PriorityTaskStorage/Fallback/FallbackTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/ModularTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/ModularTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Primary/Primitive/PrimitivePrimaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Primary/PrimitiveHeap/PrimitiveHeapPrimaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Primary/ArrayList/ArrayListPrimaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Secondary/Primitive/PrimitiveSecondaryTaskStorage.h"

#include "../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../primitives/Barrier/Simple/SimpleBarrier.h"

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

template <typename T>
class FixedSizeCircularArrayStealingDequeFallbackTaskStorage : public FallbackTaskStorage<T, FixedSizeCircularArrayStealingDeque > {
public:
	template <typename ... ConsParams>
	FixedSizeCircularArrayStealingDequeFallbackTaskStorage(ConsParams&& ... params)
	: FallbackTaskStorage<T, FixedSizeCircularArrayStealingDeque >(static_cast<ConsParams&&>(params) ...) {};
};

template <typename T>
class DefaultPrimitivePrimaryTaskStorage : public PrimitivePrimaryTaskStorage<T, FixedSizeCircularArray> {
public:
	template <typename ... ConsParams>
	DefaultPrimitivePrimaryTaskStorage(ConsParams&& ... params)
	: PrimitivePrimaryTaskStorage<T, FixedSizeCircularArray>(static_cast<ConsParams&&>(params) ...) {}
};

template <typename T>
class DefaultPrimitiveHeapPrimaryTaskStorage : public PrimitiveHeapPrimaryTaskStorage<T, FixedSizeCircularArray> {
public:
	template <typename ... ConsParams>
	DefaultPrimitiveHeapPrimaryTaskStorage(ConsParams&& ... params)
	: PrimitiveHeapPrimaryTaskStorage<T, FixedSizeCircularArray>(static_cast<ConsParams&&>(params) ...) {}
};

template <typename T>
class DefaultArrayListPrimaryTaskStorage : public ArrayListPrimaryTaskStorage<T> {
public:
	template <typename ... ConsParams>
	DefaultArrayListPrimaryTaskStorage(ConsParams&& ... params)
	: ArrayListPrimaryTaskStorage<T>(static_cast<ConsParams&&>(params) ...) {}
};

template <typename T>
class PrimitiveModularTaskStorage : public ModularTaskStorage<T, DefaultPrimitivePrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitiveModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<T, DefaultPrimitivePrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <typename T>
class PrimitiveHeapModularTaskStorage : public ModularTaskStorage<T, DefaultPrimitiveHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitiveHeapModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<T, DefaultPrimitiveHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <typename T>
class ArrayListModularTaskStorage : public ModularTaskStorage<T, DefaultArrayListPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	ArrayListModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<T, DefaultArrayListPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

typedef BasicMixedModeScheduler<OversubscribedSimpleCPUHierarchy, TwoLevelGrowingCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff>
	DefaultMixedModeScheduler;
typedef BasicScheduler<OversubscribedSimpleCPUHierarchy, FixedSizeCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff>
	DefaultBasicScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, FixedSizeCircularArrayStealingDequeFallbackTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	FallbackPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	PrimitivePriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	PrimitivePrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	PrimitivePrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 16>
	PrimitivePrioritySchedulerVeryLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	PrimitiveHeapPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	PrimitiveHeapPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	PrimitiveHeapPrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 16>
	PrimitiveHeapPrioritySchedulerVeryLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	ArrayListPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	ArrayListPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	ArrayListPrioritySchedulerLongQueues;
typedef SynchroneousScheduler<OversubscribedSimpleCPUHierarchy>
	DefaultSynchroneousScheduler;

}

#endif /* TEST_SCHEDULERS_H_ */
