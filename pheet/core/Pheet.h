/*
 * Pheet.h
 *
 *  Created on: Jan 31, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef CORE_PHEET_H_
#define CORE_PHEET_H_

#include <random>
#include <functional>

#include "../settings.h"
#include "SystemModelEnv.h"
#include "PrimitivesEnv.h"
#include "DataStructuresEnv.h"
#include "ConcurrentDataStructures.h"

namespace pheet {

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
class PheetEnv {
private:
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT> Self;
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT> ThisType;

public:
	typedef SystemModelT<Self> SystemModel;
	typedef PrimitivesT<Self> Primitives;
	typedef DataStructuresT<Self> DataStructures;
	typedef DataStructures DS;
	typedef ConcurrentDataStructuresT<Self> ConcurrentDataStructures;
	typedef ConcurrentDataStructures CDS;

	typedef typename SystemModel::MachineModel MachineModel;

	typedef typename Primitives::Backoff Backoff;
	typedef typename Primitives::Barrier Barrier;

	typedef SchedulerT<Self> Scheduler;
	typedef Scheduler Environment;
	typedef typename Scheduler::Place Place;
	typedef typename Scheduler::Task Task;
	template <typename F>
		using FunctorTask = typename Scheduler::template FunctorTask<F>;
	typedef typename Scheduler::Finish Finish;

	template<template <class P> class NewSched>
	using WithScheduler = PheetEnv<NewSched, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>;

	template<template <class P> class NewMM>
	using WithMachineModel = PheetEnv<SchedulerT, SystemModel::template WithMachineModel<NewMM>::template T, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>;


	PheetEnv() {}
	~PheetEnv() {}

	template<class CallTaskType, typename ... TaskParams>
		static void finish(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		static void finish(F&& f, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		static void call(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		static void call(F&& f, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		static void spawn(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		static void spawn(F&& f, TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		static void spawn_prio(Strategy s, TaskParams&& ... params);

	template<class Strategy, typename F, typename ... TaskParams>
		static void spawn_prio(Strategy s, F&& f, TaskParams&& ... params);

	static std::mt19937& get_rng();
	template <typename IntT> static IntT rand_int(IntT max);
	template <typename IntT> static IntT rand_int(IntT min, IntT max);
	static Place* get_place();
	static procs_t get_place_id();

private:

};


template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::finish(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->finish<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<typename F, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::finish(F&& f, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->finish(f, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::spawn(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->spawn<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<typename F, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::spawn(F&& f, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->spawn(f, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<class CallTaskType, class Strategy, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::spawn_prio(Strategy s, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->spawn_prio<CallTaskType>(s, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<class Strategy, typename F, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::spawn_prio(Strategy s, F&& f, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->spawn_prio(s, f, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::call(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template<typename F, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::call(F&& f, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	p->call(f, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
inline std::mt19937& PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::get_rng() {
	Place* p = Scheduler::get_place();
	pheet_assert(p != NULL);
	return p->get_rng();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
inline
typename PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::Place*
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::get_place() {
	return Scheduler::get_place();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
inline
procs_t
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::get_place_id() {
	Place* place = get_place();
	if(place == nullptr) {
		return 0;
	}
	return place->get_id();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template <typename IntT>
inline
IntT
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::rand_int(IntT max) {
	std::uniform_int_distribution<IntT> dist(0, max);
	return dist(get_rng());
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT, template <class Env> class ConcurrentDataStructuresT>
template <typename IntT>
inline
IntT
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT, ConcurrentDataStructuresT>::rand_int(IntT min, IntT max) {
	std::uniform_int_distribution<IntT> dist(min, max);
	return dist(get_rng());
}

}

#include "../sched/Priority/PriorityScheduler.h"

namespace pheet {
typedef PheetEnv<PriorityScheduler, SystemModel, Primitives, DataStructures, ConcurrentDataStructures> Pheet;

}

#endif /* CORE_PHEET_H_ */
