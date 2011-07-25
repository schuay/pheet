/*
 * SynchroneousSchedulerTaskExecutionContext.h
 *
 *  Created on: 19.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_
#define SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_

namespace pheet {

template <class Scheduler>
class SynchroneousSchedulerTaskExecutionContext {
public:
	SynchroneousSchedulerTaskExecutionContext(Scheduler *sched);
	~SynchroneousSchedulerTaskExecutionContext();

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
SynchroneousSchedulerTaskExecutionContext<Scheduler>::SynchroneousSchedulerTaskExecutionContext(Scheduler *sched)
: sched(sched){

}

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>::~SynchroneousSchedulerTaskExecutionContext() {

}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::call(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::spawn(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}

template <class Scheduler>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::local_finish(TaskParams ... params) {
	sched->finish<CallTaskType>(params ...);
}


}

#endif /* SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_ */
