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

template <class CPUHierarchy, bool PRE_INITIALIZE>
class SequentialTaskMixedModeScheduler {
public:
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler> Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContext<CPUHierarchy, PRE_INITIALIZE> TaskExecutionContext;

	SequentialTaskMixedModeScheduler(CPUHierarchy& cpus);
	~SequentialTaskMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

private:
	bool finished;
	TaskExecutionContext *threads;
};

}

#endif /* SEQUENTIALTASKMIXEDMODESCHEDULER_H_ */
