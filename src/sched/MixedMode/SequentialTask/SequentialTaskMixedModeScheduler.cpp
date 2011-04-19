/*
 * SequentialTaskMixedModeScheduler.cpp
 *
 *  Created on: 13.04.2011
  *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "SequentialTaskMixedModeScheduler.h"

namespace pheet {

SequentialTaskMixedModeSchedulerState::SequentialTaskMixedModeSchedulerState()
: current_state(0), startup_task(NULL) {

}

}
