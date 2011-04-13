/*
 * SequentialTaskMixedModeScheduler.h
 *
 *  Created on: 13.04.2011
 *      Author: mwimmer
 */

#ifndef SEQUENTIALTASKMIXEDMODESCHEDULER_H_
#define SEQUENTIALTASKMIXEDMODESCHEDULER_H_

#include "SequentialTaskMixedModeSchedulerTaskExecutionContext.h"

namespace pheet {

template <class CPUHierarchy>
class SequentialTaskMixedModeScheduler {
public:
	typedef SynchroneousMixedModeTask<SequentialTaskMixedModeScheduler> Task;
	typedef SequentialTaskMixedModeSchedulerTaskExecutionContext TaskExecutionContext;

	SequentialTaskMixedModeScheduler();
	~SequentialTaskMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

private:


	TaskExecutionContext *threads;
};

}

#endif /* SEQUENTIALTASKMIXEDMODESCHEDULER_H_ */
