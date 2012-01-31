/*
 * Pheet.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PHEET_H_
#define PHEET_H_

#include <random>
#include <functional>

#include "../settings.h"
#include "../sched/Basic/Priority/PriorityScheduler.h"
#include "PrimitivesEnv.h"
#include "DataStructuresEnv.h"


namespace pheet {

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
class PheetEnv {
private:
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT> Self;
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT> ThisType;

	typedef SchedulerT<Self> Scheduler;
	typedef SystemModelT<Self> SystemModel;
	typedef PrimitivesT<Self> Primitives;
	typedef DataStructuresT<Self> DataStructures;

public:
	typedef Scheduler Environment;
	typedef typename Scheduler::Place Place;
	typedef typename Scheduler::Task Task;
	typedef typename Scheduler::Finish Finish;

	typedef typename SystemModel::MachineModel MachineModel;

	typedef typename DataStructures::Backoff Backoff;
	typedef typename DataStructures::Barrier Barrier;

	PheetEnv() {}
	~PheetEnv() {}

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		void spawn_prio(Strategy s, TaskParams&& ... params);

	std::mt19937& get_rng();
	template <typename IntT> IntT rand_int(IntT max);
	template <typename IntT> IntT rand_int(IntT min, IntT max);
	Place get_place();

private:

};


template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::finish(TaskParams&& ... params) {
	Place* p = Scheduler::get_context();
	assert(p != NULL);
	p->finish<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::spawn(TaskParams&& ... params) {
	Place* p = Scheduler::get_context();
	assert(p != NULL);
	p->spawn<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, class Strategy, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::spawn_prio(Strategy s, TaskParams&& ... params) {
	Place* p = Scheduler::get_context();
	assert(p != NULL);
	p->spawn_prio<CallTaskType>(s, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::call(TaskParams&& ... params) {
	Place* p = Scheduler::get_context();
	assert(p != NULL);
	p->call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
inline std::mt19937& get_rng() {
	Place* p = Scheduler::get_context();
	assert(p != NULL);
	return p->get_rng();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
inline
typename PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::Place
PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::get_place() {
	return Scheduler::get_context();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template <typename IntT>
inline
IntT
PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::rand_int(IntT max) {
	std::uniform_int_distribution<IntT> dist(0, max);
	return dist(get_rng());
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template <typename IntT>
inline
IntT
PrioritySchedulerTaskExecutionContext<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::rand_int(IntT min, IntT max) {
	std::uniform_int_distribution<IntT> dist(min, max);
	return dist(get_rng());
}

typedef PheetEnv<PriorityScheduler, PheetPrimitives, PheetDataStructures> Pheet;

}

#endif /* PHEET_H_ */
