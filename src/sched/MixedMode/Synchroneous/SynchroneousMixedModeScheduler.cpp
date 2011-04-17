/*
 * SynchroneousMixedModeScheduler.cpp
 *
 *  Created on: 10.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SynchroneousMixedModeScheduler.h"

namespace pheet {

char const SynchroneousMixedModeScheduler::name[] = "SynchroneousMixedModeScheduler";


SynchroneousMixedModeScheduler::SynchroneousMixedModeScheduler()
: tec(this){

}

SynchroneousMixedModeScheduler::~SynchroneousMixedModeScheduler() {

}


SynchroneousMixedModeSchedulerTaskExecutionContext::SynchroneousMixedModeSchedulerTaskExecutionContext(SynchroneousMixedModeScheduler *sched)
: sched(sched){

}

SynchroneousMixedModeSchedulerTaskExecutionContext::~SynchroneousMixedModeSchedulerTaskExecutionContext() {

}

}
