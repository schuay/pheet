/*
 * SortingTests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SortingTests.h"

#include "../../sched/MixedMode/Synchroneous/SynchroneousMixedModeScheduler.h"

#include "Reference/ReferenceSTLSort.h"
#include "MixedMode/MixedModeForkJoinQuicksort.h"

#include <iostream>

namespace pheet {

SortingTests::SortingTests() {

}

SortingTests::~SortingTests() {

}

void SortingTests::run_test() {
	std::cout << "----" << std::endl;
	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttime\truns" << std::endl;

	this->run_sorter<MixedModeForkJoinQuicksort<SynchroneousMixedModeScheduler> >();
	this->run_sorter<ReferenceSTLSort>();
}

}
