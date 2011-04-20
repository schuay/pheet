/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SortingTests.h"

#include "../../sched/MixedMode/Synchroneous/SynchroneousMixedModeScheduler.h"
#include "../../sched/MixedMode/SequentialTask/SequentialTaskMixedModeScheduler.h"

#include "Reference/ReferenceSTLSort.h"
#include "MixedMode/MixedModeForkJoinQuicksort.h"

#include "../../em/CPUHierarchy/Simple/SimpleCPUHierarchy.h"

#include "../../ds/CircularArray/FixedSize/FixedSizeCircularArray.h"
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
};

template <typename T>
FixedSizeCircularArrayStealingDeque<T>::FixedSizeCircularArrayStealingDeque(size_t initial_capacity)
: CircularArrayStealingDeque<T, FixedSizeCircularArray >(initial_capacity){

}

void SortingTests::run_test() {
	std::cout << "----" << std::endl;
	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttime\truns" << std::endl;

	this->run_sorter<MixedModeForkJoinQuicksort<SequentialTaskMixedModeScheduler<SimpleCPUHierarchy, FixedSizeCircularArrayStealingDeque, SimpleBarrier<StandardExponentialBackoff>, StandardExponentialBackoff> > >();
	this->run_sorter<MixedModeForkJoinQuicksort<SynchroneousMixedModeScheduler<SimpleCPUHierarchy> > >();
	this->run_sorter<ReferenceSTLSort>();
}

}
