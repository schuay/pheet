/*
 * GraphBipartitioningTests.cpp
 *
 *  Created on: 07.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "GraphBipartitioningTests.h"

#include "../test_schedulers.h"

namespace pheet {

GraphBipartitioningTests::GraphBipartitioningTests() {

}

GraphBipartitioningTests::~GraphBipartitioningTests() {

}

void SortingTests::run_test() {
	std::cout << "----" << std::endl;

	this->run_partitioner<MixedModeQuicksort<DefaultMixedModeScheduler> >();
	this->run_partitioner<DagQuicksort<DefaultMixedModeScheduler> >();
	this->run_partitioner<DagQuicksort<DefaultBasicScheduler> >();
	this->run_partitioner<DagQuicksort<DefaultSynchroneousScheduler> >();
	this->run_partitioner<ReferenceQuicksort>();
	this->run_partitioner<ReferenceSTLSort>();
}

}
