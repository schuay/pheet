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
#include "../ds/PriorityTaskStorage/Modular/Primary/ArrayListHeap/ArrayListHeapPrimaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Secondary/Primitive/PrimitiveSecondaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Secondary/MultiSteal/MultiStealSecondaryTaskStorage.h"

#include "../ds/PriorityQueue/Heap/Heap.h"
#include "../ds/PriorityQueue/SortedArrayHeap/SortedArrayHeap.h"

#include "../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../primitives/Barrier/Simple/SimpleBarrier.h"

namespace pheet {

//using FixedSizeCircularArrayStealingDeque = CircularArrayStealingDeque<T, FixedSizeCircularArray<T> >;
template <class Scheduler, typename T>
class FixedSizeCircularArrayStealingDeque : public CircularArrayStealingDeque<Scheduler, T, FixedSizeCircularArray > {
public:
	FixedSizeCircularArrayStealingDeque(size_t initial_capacity);
	FixedSizeCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas);
};

template <class Scheduler, typename T>
FixedSizeCircularArrayStealingDeque<Scheduler, T>::FixedSizeCircularArrayStealingDeque(size_t initial_capacity)
: CircularArrayStealingDeque<Scheduler, T, FixedSizeCircularArray >(initial_capacity){

}

template <class Scheduler, typename T>
FixedSizeCircularArrayStealingDeque<Scheduler, T>::FixedSizeCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas)
: CircularArrayStealingDeque<Scheduler, T, FixedSizeCircularArray >(initial_capacity, num_stolen, num_pop_cas){

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

template <class Scheduler, typename T>
class TwoLevelGrowingCircularArrayStealingDeque : public CircularArrayStealingDeque<Scheduler, T, MyTwoLevelGrowingCircularArray > {
public:
	TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity);
	TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas);
};

template <class Scheduler, typename T>
TwoLevelGrowingCircularArrayStealingDeque<Scheduler, T>::TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity)
: CircularArrayStealingDeque<Scheduler, T, MyTwoLevelGrowingCircularArray >(initial_capacity){

}

template <class Scheduler, typename T>
TwoLevelGrowingCircularArrayStealingDeque<Scheduler, T>::TwoLevelGrowingCircularArrayStealingDeque(size_t initial_capacity, BasicPerformanceCounter<Scheduler, stealing_deque_count_steals>& num_stolen, BasicPerformanceCounter<Scheduler, stealing_deque_count_pop_cas>& num_pop_cas)
: CircularArrayStealingDeque<Scheduler, T, MyTwoLevelGrowingCircularArray >(initial_capacity, num_stolen, num_pop_cas){

}

template <class Scheduler, typename T>
class FixedSizeCircularArrayStealingDequeFallbackTaskStorage : public FallbackTaskStorage<Scheduler, T, FixedSizeCircularArrayStealingDeque > {
public:
	template <typename ... ConsParams>
	FixedSizeCircularArrayStealingDequeFallbackTaskStorage(ConsParams&& ... params)
	: FallbackTaskStorage<Scheduler, T, FixedSizeCircularArrayStealingDeque >(static_cast<ConsParams&&>(params) ...) {};
};

template <class Scheduler, typename T>
class DefaultPrimitivePrimaryTaskStorage : public PrimitivePrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray> {
public:
	template <typename ... ConsParams>
	DefaultPrimitivePrimaryTaskStorage(ConsParams&& ... params)
	: PrimitivePrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class DefaultPrimitiveHeapPrimaryTaskStorage : public PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray> {
public:
	template <typename ... ConsParams>
	DefaultPrimitiveHeapPrimaryTaskStorage(ConsParams&& ... params)
	: PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitivePheetHeapPrimaryTaskStorage : public PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray, Heap> {
public:
	template <typename ... ConsParams>
	PrimitivePheetHeapPrimaryTaskStorage(ConsParams&& ... params)
	: PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray, Heap>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitiveSortedArrayHeapPrimaryTaskStorage : public PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray, SortedArrayHeap> {
public:
	template <typename ... ConsParams>
	PrimitiveSortedArrayHeapPrimaryTaskStorage(ConsParams&& ... params)
	: PrimitiveHeapPrimaryTaskStorage<Scheduler, T, FixedSizeCircularArray, SortedArrayHeap>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class DefaultArrayListPrimaryTaskStorage : public ArrayListPrimaryTaskStorage<Scheduler, T> {
public:
	template <typename ... ConsParams>
	DefaultArrayListPrimaryTaskStorage(ConsParams&& ... params)
	: ArrayListPrimaryTaskStorage<Scheduler, T>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class DefaultArrayListHeapPrimaryTaskStorage : public ArrayListHeapPrimaryTaskStorage<Scheduler, T> {
public:
	template <typename ... ConsParams>
	DefaultArrayListHeapPrimaryTaskStorage(ConsParams&& ... params)
	: ArrayListHeapPrimaryTaskStorage<Scheduler, T>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitiveModularTaskStorage : public ModularTaskStorage<Scheduler, T, DefaultPrimitivePrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitiveModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, DefaultPrimitivePrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitiveHeapModularTaskStorage : public ModularTaskStorage<Scheduler, T, DefaultPrimitiveHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitiveHeapModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, DefaultPrimitiveHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitivePheetHeapModularTaskStorage : public ModularTaskStorage<Scheduler, T, PrimitivePheetHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitivePheetHeapModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, PrimitivePheetHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class PrimitiveSortedArrayHeapModularTaskStorage : public ModularTaskStorage<Scheduler, T, PrimitiveSortedArrayHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	PrimitiveSortedArrayHeapModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, PrimitiveSortedArrayHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class ArrayListModularTaskStorage : public ModularTaskStorage<Scheduler, T, DefaultArrayListPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	ArrayListModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, DefaultArrayListPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class ArrayListHeapModularTaskStorage : public ModularTaskStorage<Scheduler, T, DefaultArrayListHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	ArrayListHeapModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, DefaultArrayListHeapPrimaryTaskStorage, PrimitiveSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
};

template <class Scheduler, typename T>
class ArrayListHeapMultiStealModularTaskStorage : public ModularTaskStorage<Scheduler, T, DefaultArrayListHeapPrimaryTaskStorage, MultiStealSecondaryTaskStorage > {
public:
	template <typename ... ConsParams>
	ArrayListHeapMultiStealModularTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Scheduler, T, DefaultArrayListHeapPrimaryTaskStorage, MultiStealSecondaryTaskStorage >(static_cast<ConsParams&&>(params) ...) {}
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
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitivePheetHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	PrimitivePheetHeapPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitivePheetHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	PrimitivePheetHeapPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitivePheetHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	PrimitivePheetHeapPrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitivePheetHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 16>
	PrimitivePheetHeapPrioritySchedulerVeryLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveSortedArrayHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	PrimitiveSortedArrayHeapPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveSortedArrayHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	PrimitiveSortedArrayHeapPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, PrimitiveSortedArrayHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	PrimitiveSortedArrayHeapPrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	ArrayListPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	ArrayListPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 6>
	ArrayListPrioritySchedulerMediumQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	ArrayListPrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 16>
	ArrayListPrioritySchedulerVeryLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy>
	ArrayListHeapPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 1>
	ArrayListHeapPrioritySchedulerShortQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 6>
	ArrayListHeapPrioritySchedulerMediumQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 8>
	ArrayListHeapPrioritySchedulerLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 16>
	ArrayListHeapPrioritySchedulerVeryLongQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapMultiStealModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 3>
	ArrayListHeapMultiStealPriorityScheduler;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapMultiStealModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 4>
	ArrayListHeapMultiStealPrioritySchedulerSlightlyLongerQueues;
typedef PriorityScheduler<OversubscribedSimpleCPUHierarchy, ArrayListHeapMultiStealModularTaskStorage, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff, LifoFifoStrategy, 6>
	ArrayListHeapMultiStealPrioritySchedulerLongQueues;
typedef SynchroneousScheduler<OversubscribedSimpleCPUHierarchy>
	DefaultSynchroneousScheduler;

}

#endif /* TEST_SCHEDULERS_H_ */
