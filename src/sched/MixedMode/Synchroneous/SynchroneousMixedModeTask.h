/*
 * SynchroneousMixedModeTask.h
 *
 *  Created on: 06.04.2011
 *      Author: mwimmer
 */

#ifndef SYNCHRONEOUSMIXEDMODETASK_H_
#define SYNCHRONEOUSMIXEDMODETASK_H_

#include "../../../misc/types.h"

template <class Scheduler>
class SynchroneousMixedModeTask {
public:
	SynchroneousMixedModeTask();
	virtual ~SynchroneousMixedModeTask();

	procs_t get_num_threads();

protected:
	virtual void execute(Scheduler::TaskExecutionContextType &tec);

private:
};

template <class Scheduler>
SynchroneousMixedModeTask<Scheduler>::SynchroneousMixedModeTask() {

}

template <class Scheduler>
SynchroneousMixedModeTask<Scheduler>::~SynchroneousMixedModeTask() {

}


template <class Scheduler>
procs_t SynchroneousMixedModeTask<Scheduler>::get_num_threads() {
	return 1;
}

template <class Scheduler>
void SynchroneousMixedModeTask<Scheduler>::execute(Scheduler::TaskExecutionContextType &tec) {

}

#endif /* SYNCHRONEOUSMIXEDMODETASK_H_ */
