/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SortingTests.h"

#ifdef SORTING_TEST
#include "Reference/ReferenceSTLSort.h"
#include "Reference/ReferenceQuicksort.h"
#include "Dag/DagQuicksort.h"
//#include "MixedMode/MixedModeQuicksort.h"
#include "Reference/ReferenceHeapSort.h"

#include <pheet/ds/PriorityQueue/Heap/Heap.h>
#include <pheet/ds/PriorityQueue/STLPriorityQueueWrapper/STLPriorityQueueWrapper.h>
#include <pheet/ds/PriorityQueue/SortedArrayHeap/SortedArrayHeap.h>

#include <pheet/pheet.h>
#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/sched/Synchroneous/SynchroneousScheduler.h>

#include <iostream>
#endif

namespace pheet {


SortingTests::SortingTests() {

}

SortingTests::~SortingTests() {

}

void SortingTests::run_test() {
#ifdef SORTING_TEST
	std::cout << "----" << std::endl;
//std::cout << Pheet::Environment::max_cpus << std::endl;
	this->run_sorter<	Pheet,
						DagQuicksort>();
	this->run_sorter<	Pheet::WithScheduler<BasicScheduler>,
						DagQuicksort>();
	this->run_sorter<	Pheet::WithScheduler<SynchroneousScheduler>,
						DagQuicksort>();
	this->run_sorter<	Pheet::WithScheduler<SynchroneousScheduler>,
						ReferenceSTLSort>();
	this->run_sorter<	Pheet::WithScheduler<SynchroneousScheduler>,
						ReferenceQuicksort>();
	this->run_sorter<	Pheet::WithScheduler<SynchroneousScheduler>,
						ReferenceHeapSort>();

/*	this->run_sorter<MixedModeQuicksort<DefaultMixedModeScheduler> >();
	this->run_sorter<DagQuicksort<DefaultMixedModeScheduler> >();
	this->run_sorter<DagQuicksort<DefaultBasicScheduler> >();
	this->run_sorter<DagQuicksort<PrimitiveHeapPriorityScheduler> >();
	this->run_sorter<DagQuicksort<PrimitivePriorityScheduler> >();
	this->run_sorter<DagQuicksort<FallbackPriorityScheduler> >();
	this->run_sorter<DagQuicksort<DefaultSynchroneousScheduler> >();
	this->run_sorter<ReferenceQuicksort>();
	this->run_sorter<ReferenceSTLSort>();
	this->run_sorter<ReferenceHeapSort<STLPriorityQueueWrapper> >();
	this->run_sorter<ReferenceHeapSort<Heap> >();*/
//	this->run_sorter<ReferenceHeapSort<SortedArrayHeap> >();
#endif
}


}
