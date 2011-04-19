/*
 * SynchroneousMixedModeSchedulerTaskExecutionContext.h
 *
 *  Created on: 19.04.2011
 *      Author: mwimmer
 */

#ifndef SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_
#define SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_

namespace pheet {

template <class Scheduler>
class SynchroneousMixedModeSchedulerTaskExecutionContext {
public:
	SynchroneousMixedModeSchedulerTaskExecutionContext(Scheduler *sched);
	~SynchroneousMixedModeSchedulerTaskExecutionContext();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void local_finish(TaskParams ... params);

private:
	Scheduler* sched;
};

template <class Scheduler>
SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::SynchroneousMixedModeSchedulerTaskExecutionContext(Scheduler *sched)
: sched(sched){

}

template <class Scheduler>
SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::~SynchroneousMixedModeSchedulerTaskExecutionContext() {

}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::call(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::spawn(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousMixedModeSchedulerTaskExecutionContext<Scheduler>::local_finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}


}

#endif /* SYNCHRONEOUSMIXEDMODESCHEDULERTASKEXECUTIONCONTEXT_H_ */
