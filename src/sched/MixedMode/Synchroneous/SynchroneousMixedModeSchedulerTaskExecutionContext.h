/*
 * SynchroneousMixedModeSchedulerTaskExecutionContext.h
 *
 *  Created on: 06.04.2011
 *      Author: mwimmer
 */

#ifndef SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_
#define SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_

#include "SynchroneousMixedModeTask.h"

class SynchroneousMixedModeSchedulerTaskExecutionContext {
public:
	SynchroneousMixedModeSchedulerTaskExecutionContext(SynchroneousMixedModeScheduler *sched);
	virtual ~SynchroneousMixedModeSchedulerTaskExecutionContext();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void local_finish(TaskParams ... params);

private:
	SynchroneousMixedModeScheduler *sched;
};

SynchroneousMixedModeScheduler::SynchroneousMixedModeScheduler(SynchroneousMixedModeScheduler *sched)
: sched(sched){

}

SynchroneousMixedModeScheduler::~SynchroneousMixedModeScheduler() {

}

template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeScheduler::finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeScheduler::call(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeScheduler::spawn(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeScheduler::local_finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}
#endif /* SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
