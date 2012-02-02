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

namespace pheet {

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
class PheetEnv {
private:
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT> Self;
	typedef PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT> ThisType;

	typedef SystemModelT<Self> SystemModel;
	typedef PrimitivesT<Self> Primitives;
	typedef DataStructuresT<Self> DataStructures;

public:
	typedef typename SystemModel::MachineModel MachineModel;

	typedef typename Primitives::Backoff Backoff;
	typedef typename Primitives::Barrier Barrier;

	typedef SchedulerT<Self> Scheduler;
	typedef Scheduler Environment;
	typedef typename Scheduler::Place Place;
	typedef typename Scheduler::Task Task;
	typedef typename Scheduler::Finish Finish;

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
	Place* get_place();

private:

};


template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::finish(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	assert(p != NULL);
	p->finish<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::spawn(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	assert(p != NULL);
	p->spawn<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, class Strategy, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::spawn_prio(Strategy s, TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	assert(p != NULL);
	p->spawn_prio<CallTaskType>(s, static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template<class CallTaskType, typename ... TaskParams>
void PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::call(TaskParams&& ... params) {
	Place* p = Scheduler::get_place();
	assert(p != NULL);
	p->call<CallTaskType>(static_cast<TaskParams&&>(params) ...);
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
inline std::mt19937& PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::get_rng() {
	Place* p = Scheduler::get_place();
	assert(p != NULL);
	return p->get_rng();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
inline
typename PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::Place*
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::get_place() {
	return Scheduler::get_place();
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template <typename IntT>
inline
IntT
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::rand_int(IntT max) {
	std::uniform_int_distribution<IntT> dist(0, max);
	return dist(get_rng());
}

template <template <class Env> class SchedulerT, template <class Env> class SystemModelT, template <class Env> class PrimitivesT, template <class Env> class DataStructuresT>
template <typename IntT>
inline
IntT
PheetEnv<SchedulerT, SystemModelT, PrimitivesT, DataStructuresT>::rand_int(IntT min, IntT max) {
	std::uniform_int_distribution<IntT> dist(min, max);
	return dist(get_rng());
}

}

#include "../sched/Basic/Priority/PriorityScheduler.h"
#include "PrimitivesEnv.h"
#include "DataStructuresEnv.h"
#include "SystemModelEnv.h"
#include "../ds/PriorityTaskStorage/Modular/ModularTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Primary/ArrayListHeap/ArrayListHeapPrimaryTaskStorage.h"
#include "../ds/PriorityTaskStorage/Modular/Secondary/MultiSteal/MultiStealSecondaryTaskStorage.h"
#include "../sched/strategies/LifoFifo/LifoFifoStrategy.h"

namespace pheet {

template <class Pheet, typename TT>
class PheetPrimaryTaskStorage : public ArrayListHeapPrimaryTaskStorage<Pheet, TT> {
public:
	template <typename ... ConsParams>
	PheetPrimaryTaskStorage(ConsParams&& ... params)
	: ArrayListHeapPrimaryTaskStorage<Pheet, TT>(static_cast<ConsParams&&>(params) ...) {}
};

template <class Pheet, typename TT>
class PheetTaskStorage : public ModularTaskStorage<Pheet, TT, PheetPrimaryTaskStorage, MultiStealSecondaryTaskStorage> {
public:
	template <typename ... ConsParams>
	PheetTaskStorage(ConsParams&& ... params)
	: ModularTaskStorage<Pheet, TT, PheetPrimaryTaskStorage, MultiStealSecondaryTaskStorage>(static_cast<ConsParams&&>(params) ...) {}
};

template<class Env>
using PheetScheduler = PriorityScheduler<Env, PheetTaskStorage, LifoFifoStrategy, 3>;
/*
template <class Env>
class PheetScheduler : public PriorityScheduler<Env, PheetTaskStorage, LifoFifoStrategy, 3> {
public:
	typedef PriorityScheduler<Env, PheetTaskStorage, LifoFifoStrategy, 3> Base;
	typedef typename Base::Backoff Backoff;
	typedef typename Base::InternalMachineModel InternalMachineModel;
	typedef typename Base::Task Task;
	typedef typename Base::TaskStorageItem TaskStorageItem;
	typedef typename Base::TaskStorage TaskStorage;
	typedef typename Base::Place Place;
	typedef typename Base::State State;
	typedef typename Base::Finish Finish;
	typedef typename Base::StealerDescriptor StealerDescriptor;
	typedef typename Base::DefaultStrategy DefaultStrategy;

	PheetScheduler() {

	}

	PheetScheduler(procs_t num_places) : PriorityScheduler<Env, PheetTaskStorage, LifoFifoStrategy, 3>(num_places) {

	}

	~PheetScheduler() {}
};*/

typedef PheetEnv<PheetScheduler, PheetSystemModel, PheetPrimitives, PheetDataStructures> Pheet;

}

#endif /* CORE_PHEET_H_ */
