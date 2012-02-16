/*
 * LUPivTests.cpp
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#include "LUPivTests.h"
#include "Simple/SimpleLUPiv.h"
#include "LocalityStrategy/LocalityStrategyLUPiv.h"

#include <pheet/pheet.h>
#include <pheet/sched/Basic/BasicScheduler.h>
#include <pheet/models/MachineModel/HWLoc/HWLocSMTMachineModel.h>

#include <iostream>

namespace pheet {

LUPivTests::LUPivTests() {

}

LUPivTests::~LUPivTests() {

}

void LUPivTests::run_test() {
#ifdef LUPIV_TEST
	std::cout << "----" << std::endl;

	this->run_kernel<	Pheet,
						LocalityStrategyLUPiv>();
	this->run_kernel<	Pheet::WithMachineModel<HWLocSMTMachineModel>,
						LocalityStrategyLUPiv>();
	this->run_kernel<	Pheet,
						SimpleLUPiv>();
	this->run_kernel<	Pheet::WithScheduler<BasicScheduler>,
						SimpleLUPiv>();
#endif
}

}