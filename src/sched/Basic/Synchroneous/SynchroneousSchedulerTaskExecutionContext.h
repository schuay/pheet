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

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams ... params);

	boost::mt19937& get_rng();

private:
	void start_finish_region();
	void end_finish_region();

	Scheduler* sched;

	boost::mt19937 rng;

	friend class Scheduler::Finish;
};

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>::SynchroneousSchedulerTaskExecutionContext(Scheduler *sched)
: sched(sched){

}

template <class Scheduler>
SynchroneousSchedulerTaskExecutionContext<Scheduler>::~SynchroneousSchedulerTaskExecutionContext() {

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
