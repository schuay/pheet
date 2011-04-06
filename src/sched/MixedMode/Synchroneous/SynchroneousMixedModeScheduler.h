/*
 * SynchroneousMixedModeScheduler.h
 *
 *  Created on: 06.04.2011
 *      Author: mwimmer
 */

#ifndef SYNCHRONEOUSMIXEDMODESCHEDULER_H_
#define SYNCHRONEOUSMIXEDMODESCHEDULER_H_

#include "SynchroneousMixedModeTask.h"
#include "SynchroneousMixedModeTaskExecutionContext.h"

class SynchroneousMixedModeScheduler {
public:
	typedef typename SynchroneousMixedModeTask<SynchroneousMixedModeScheduler> Task;
	typedef typename SynchroneousMixedModeTaskExecutionContext TaskExecutionContext;

	SynchroneousMixedModeScheduler();
	virtual ~SynchroneousMixedModeScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

private:
	TaskExecutionContextType tec;
};

SynchroneousMixedModeScheduler::SynchroneousMixedModeScheduler()
: tec(this){

}

SynchroneousMixedModeScheduler::~SynchroneousMixedModeScheduler() {

}

template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeScheduler::finish(TaskParams ... params) {
	CallTaskType task(params ...);
	task.execute(tec);
}

#endif /* SYNCHRONEOUSMIXEDMODESCHEDULER_H_ */
