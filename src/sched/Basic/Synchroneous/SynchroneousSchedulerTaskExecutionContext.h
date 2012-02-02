/*
 * SynchroneousSchedulerTaskExecutionContext.h
 *
 *  Created on: 19.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_
#define SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_

#include <boost/random/uniform_int.hpp>
#include <boost/random/mersenne_twister.hpp>

namespace pheet {

template <class Scheduler>
class SynchroneousSchedulerTaskExecutionContext {
public:
	SynchroneousSchedulerTaskExecutionContext(Scheduler *sched);
	~SynchroneousSchedulerTaskExecutionContext();

	static SynchroneousSchedulerTaskExecutionContext<Scheduler>* get();

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	boost::mt19937& get_rng();

	void start_finish_region();
	void end_finish_region();

private:

	Scheduler* sched;

	boost::mt19937 rng;

	// In case a synchroneous scheduler is launched inside another one. Not a good style, but anyway...
	SynchroneousSchedulerTaskExecutionContext<Scheduler>* prev_context;

	static thread_local SynchroneousSchedulerTaskExecutionContext<Scheduler>* local_context;

//	friend class Scheduler::Finish;
};

template <class Scheduler>
thread_local SynchroneousSchedulerTaskExecutionContext<Scheduler>*
SynchroneousSchedulerTaskExecutionContext<Scheduler>::local_context = NULL;

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>::SynchroneousSchedulerTaskExecutionContext(Scheduler *sched)
: sched(sched){
	prev_context = local_context;
	local_context = this;
}

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>::~SynchroneousSchedulerTaskExecutionContext() {
	local_context = prev_context;
}

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>* SynchroneousSchedulerTaskExecutionContext<Scheduler>::get() {
	return local_context;
}

template <class Scheduler>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::start_finish_region() {
	// Nothing to do here
}

template <class Scheduler>
void SynchroneousSchedulerTaskExecutionContext<Scheduler>::end_finish_region() {
	// Nothing to do here
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
boost::mt19937& SynchroneousSchedulerTaskExecutionContext<Scheduler>::get_rng() {
	return rng;
}

}

#endif /* SYNCHRONEOUSSCHEDULERTASKEXECUTIONCONTEXT_H_ */
