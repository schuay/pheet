/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SortingTests.h"

#include "Reference/ReferenceSTLSort.h"
#include "Reference/ReferenceQuicksort.h"
#include "Dag/DagQuicksort.h"
#include "MixedMode/MixedModeQuicksort.h"
#include "Reference/ReferenceHeapSort.h"

#include "../../ds/PriorityQueue/Heap/Heap.h"
#include "../../ds/PriorityQueue/STLPriorityQueueWrapper/STLPriorityQueueWrapper.h"
#include "../../ds/PriorityQueue/SortedArrayHeap/SortedArrayHeap.h"

#include "../test_schedulers.h"

#include <iostream>

namespace pheet {

SortingTests::SortingTests() {

}

SortingTests::~SortingTests() {

}

void SortingTests::run_test() {
	if(sorting_test) {
		std::cout << "----" << std::endl;

		this->run_sorter<MixedModeQuicksort<DefaultMixedModeScheduler> >();
		this->run_sorter<DagQuicksort<DefaultMixedModeScheduler> >();
		this->run_sorter<DagQuicksort<DefaultBasicScheduler> >();
		this->run_sorter<DagQuicksort<PrimitiveHeapPriorityScheduler> >();
		this->run_sorter<DagQuicksort<PrimitivePriorityScheduler> >();
		this->run_sorter<DagQuicksort<FallbackPriorityScheduler> >();
		this->run_sorter<DagQuicksort<DefaultSynchroneousScheduler> >();
		this->run_sorter<ReferenceQuicksort>();
		this->run_sorter<ReferenceSTLSort>();
		this->run_sorter<ReferenceHeapSort<STLPriorityQueueWrapper> >();
		this->run_sorter<ReferenceHeapSort<Heap> >();
		this->run_sorter<ReferenceHeapSort<SortedArrayHeap> >();
	}
}

}
