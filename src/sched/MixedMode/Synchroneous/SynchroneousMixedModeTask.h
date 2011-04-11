/*
 * SynchroneousMixedModeTask.h
 *
 *  Created on: 06.04.2011
 *      Author: mwimmer
 */

#ifndef SYNCHRONEOUSMIXEDMODETASK_H_
#define SYNCHRONEOUSMIXEDMODETASK_H_

#include "../../../misc/types.h"

namespace pheet {

template <class Sched>
class SynchroneousMixedModeTask {
public:
	typedef Sched Scheduler;

	SynchroneousMixedModeTask();
	virtual ~SynchroneousMixedModeTask();

	procs_t get_num_threads();

	virtual void execute(typename Scheduler::TaskExecutionContext &tec);

private:
};

template <class Sched>
SynchroneousMixedModeTask<Sched>::SynchroneousMixedModeTask() {

}

template <class Sched>
SynchroneousMixedModeTask<Sched>::~SynchroneousMixedModeTask() {

}


template <class Sched>
procs_t SynchroneousMixedModeTask<Sched>::get_num_threads() {
	return 1;
}

template <class Sched>
void SynchroneousMixedModeTask<Sched>::execute(typename Sched::TaskExecutionContext &tec) {

}

}

#endif /* SYNCHRONEOUSMIXEDMODETASK_H_ */
