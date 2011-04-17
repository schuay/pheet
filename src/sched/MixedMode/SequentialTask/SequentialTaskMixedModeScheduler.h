/*
 * SequentialTaskMixedModeScheduler.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SEQUENTIALTASKMIXEDMODESCHEDULER_H_
#define SEQUENTIALTASKMIXEDMODESCHEDULER_H_

#include "SequentialTaskMixedModeSchedulerTaskExecutionContext.h"

namespace pheet {

template <class CPUHierarchy, class StealingDeque>
class SequentialTaskMixedModeScheduler {
public:
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler> Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, StealingDeque> TaskExecutionContext;

	SequentialTaskMixedModeScheduler(CPUHierarchy& cpus);
	~SequentialTaskMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

	static char const name[];

private:
	bool finished;
	TaskExecutionContext *threads;
};

template <class CPUHierarchy, class StealingDeque>
char const SequentialTaskMixedModeScheduler<CPUHierarchy, StealingDeque>::name[] = "SequentialTaskMixedModeScheduler";

}

#endif /* SEQUENTIALTASKMIXEDMODESCHEDULER_H_ */
