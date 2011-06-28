/*
 * BasicMixedModeTask.h
 *
 *  Created on: 16.06.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef BASICMIXEDMODETASK_H_
#define BASICMIXEDMODETASK_H_

#include "../../../misc/types.h"

/*
 *
 */
namespace pheet {

template <class Sched>
class BasicMixedModeTask {
public:
	typedef Sched Scheduler;

	BasicMixedModeTask();
	BasicMixedModeTask(procs_t num_threads);
	virtual ~BasicMixedModeTask();

	procs_t get_num_threads();

	virtual void operator()(typename Scheduler::TaskExecutionContext &tec) = 0;

private:
	procs_t num_threads;
};

template <class Sched>
BasicMixedModeTask<Sched>::BasicMixedModeTask()
:num_threads(1) {

}

template <class Sched>
BasicMixedModeTask<Sched>::BasicMixedModeTask(procs_t num_threads)
:num_threads(num_threads) {

}

template <class Sched>
BasicMixedModeTask<Sched>::~BasicMixedModeTask() {

}


template <class Sched>
procs_t BasicMixedModeTask<Sched>::get_num_threads() {
	return num_threads;
}
}

#endif /* BASICMIXEDMODETASK_H_ */
