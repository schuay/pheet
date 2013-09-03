/*
 * SsspTests.cpp
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#include "SsspTests.h"
#include "Simple/SimpleSssp.h"
#include "Strategy/StrategySssp.h"
#include "Adaptive/AdaptiveSssp.h"
#include "Reference/ReferenceSssp.h"
#include "Analysis/SsspAnalysis.h"

#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/sched/Strategy/StrategyScheduler.h>
#include <pheet/sched/BStrategy/BStrategyScheduler.h>
#include <pheet/sched/Synchroneous/SynchroneousScheduler.h>
#include <pheet/ds/StrategyTaskStorage/CentralK/CentralKStrategyTaskStorage.h>
#include <pheet/ds/StrategyTaskStorage/DistK/DistKStrategyTaskStorage.h>

namespace pheet {

SsspTests::SsspTests() {

}

SsspTests::~SsspTests() {

}

void SsspTests::run_test() {
#ifdef SSSP_SIM
	// Simulator
	this->run_algorithm<	Pheet::WithScheduler<SynchroneousScheduler>,
							SsspAnalysis>();
	return;
#endif
	std::cout << "----" << std::endl;

	this->run_algorithm<	Pheet::WithScheduler<SynchroneousScheduler>,
							ReferenceSssp>();
//	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorage>,
//							AdaptiveSssp>();
	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorage>,
							StrategySssp>();
	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<DistKStrategyTaskStorageLocalK>,
							StrategySssp>();
	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<CentralKStrategyTaskStorage>,
							StrategySssp>();
//  this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>,
//							AdaptiveSssp>();
//	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<CentralKStrategyTaskStorageMergeHeap>,
//							StrategySssp>();
	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>::WithTaskStorage<CentralKStrategyTaskStorageLocalK>,
							StrategySssp>();
	this->run_algorithm<	Pheet::WithScheduler<StrategyScheduler>,
							StrategySssp>();
/*
	this->run_algorithm<	Pheet::WithScheduler<BasicScheduler>,
							StrategySssp>();
	this->run_algorithm<	Pheet::WithScheduler<StrategyScheduler>,
							SimpleSssp>();
	this->run_algorithm<	Pheet::WithScheduler<BasicScheduler>,
							SimpleSssp>();
							*/
}

} /* namespace pheet */
