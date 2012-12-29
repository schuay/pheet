/*
 * SsspTests.cpp
 *
 *  Created on: Jul 16, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#include "SsspTests.h"
#ifdef SSSP_TEST
#include "Simple/SimpleSssp.h"
#include "Strategy/StrategySssp.h"
#include "Reference/ReferenceSssp.h"

#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/sched/Strategy/StrategyScheduler.h>
#include <pheet/sched/BStrategy/BStrategyScheduler.h>
#include <pheet/sched/Synchroneous/SynchroneousScheduler.h>
#endif

namespace pheet {

SsspTests::SsspTests() {

}

SsspTests::~SsspTests() {

}

void SsspTests::run_test() {
#ifdef SSSP_TEST
	std::cout << "----" << std::endl;

	this->run_algorithm<	Pheet::WithScheduler<SynchroneousScheduler>,
							ReferenceSssp>();
	this->run_algorithm<	Pheet::WithScheduler<BStrategyScheduler>,
							StrategySssp>();
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
#endif
}

} /* namespace pheet */
