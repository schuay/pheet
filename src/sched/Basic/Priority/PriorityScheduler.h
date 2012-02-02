/*
 * PriorityScheduler.h
 *
 *  Created on: 19.09.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PRIORITYSCHEDULER_H_
#define PRIORITYSCHEDULER_H_

#include "../../../settings.h"
#include "../../common/SchedulerTask.h"
#include "../../common/FinishRegion.h"
#include "PrioritySchedulerPlace.h"
#include "PrioritySchedulerStealerDescriptor.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../models/MachineModel/BinaryTree/BinaryTreeMachineModel.h"

#include <stdint.h>
#include <limits>
#include <vector>
#include <iostream>

namespace pheet {

template <class Pheet>
struct PrioritySchedulerState {
	PrioritySchedulerState();

	uint8_t current_state;
	typename Pheet::Barrier state_barrier;
	typename Pheet::Task* startup_task;
};

template <class Pheet>
PrioritySchedulerState<Pheet>::PrioritySchedulerState()
: current_state(0), startup_task(NULL) {

}


template <class Pheet>
struct PrioritySchedulerTaskStorageItem {
	typedef typename Pheet::Place Place;

	PrioritySchedulerTaskStorageItem();

	typename Pheet::Place::Task* task;
	typename Pheet::Place::StackElement* stack_element;

	bool operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
	bool operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
};

template <class Pheet>
PrioritySchedulerTaskStorageItem<Pheet>::PrioritySchedulerTaskStorageItem()
: task(NULL), stack_element(NULL) {

}

template <class Pheet>
bool PrioritySchedulerTaskStorageItem<Pheet>::operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task == task;
}

template <class Pheet>
bool PrioritySchedulerTaskStorageItem<Pheet>::operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task != task;
}


template <class Pheet>
class nullable_traits<PrioritySchedulerTaskStorageItem<Pheet> > {
public:
	static PrioritySchedulerTaskStorageItem<Pheet> const null_value;
};

template <class Pheet>
PrioritySchedulerTaskStorageItem<Pheet> const nullable_traits<PrioritySchedulerTaskStorageItem<Pheet> >::null_value;

/*
 * May only be used once
 */
template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold = 4>
class PriorityScheduler {
public:
	typedef typename Pheet::Backoff Backoff;
	typedef typename Pheet::MachineModel MachineModel;
	typedef BinaryTreeMachineModel<Pheet, MachineModel> InternalMachineModel;
	typedef PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold> Self;
	typedef SchedulerTask<Pheet> Task;
	typedef PrioritySchedulerTaskStorageItem<Pheet> TaskStorageItem;
	typedef TaskStorageT<Pheet, TaskStorageItem> TaskStorage;
	typedef PrioritySchedulerPlace<Pheet, CallThreshold> Place;
	typedef PrioritySchedulerState<Pheet> State;
	typedef FinishRegion<Pheet> Finish;
	typedef PrioritySchedulerStealerDescriptor<Pheet> StealerDescriptor;
	typedef DefaultStrategyT<Pheet> DefaultStrategy;

	/*
	 * Uses complete machine
	 */
	PriorityScheduler();

	/*
	 * Only uses the given number of places
	 * (Currently no direct support for oversubscription)
	 */
	PriorityScheduler(procs_t num_places);
	~PriorityScheduler();

	static void print_name();

	void print_performance_counter_values();

	void print_performance_counter_headers();

	static Place* get_place();
	static procs_t get_place_id();
	Place* get_place_at(procs_t place_id);

	static char const name[];
	static procs_t const max_cpus;

private:
	void initialize_places(InternalMachineModel* cpus, size_t offset, std::vector<typename Place::LevelDescription*>* levels);

	InternalMachineModel machine_model;
	Place** places;
	procs_t num_places;

	State state;

	typename Place::PerformanceCounters performance_counters;
};

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
char const PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::name[] = "PriorityScheduler";

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
procs_t const PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler()
: num_places(machine_model.get_num_leaves()) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, state, performance_counters);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler(procs_t num_places)
: num_places(num_places) {

	places = new Place*[num_places];

	std::vector<typename Place::LevelDescription*> levels;
	initialize_places(&machine_model, 0, &levels);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::~PriorityScheduler() {
	delete places[0];
	delete[] places;
}
/*
template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::initialize_places(InternalMachineModel* ch, size_t offset, std::vector<typename Place::LevelDescription*>* levels) {
	if(ch->get_size() > 1) {
		std::vector<InternalMachineModel*> const* sub = ch->get_subsets();

		if(sub->size() == 2) {
			InternalMachineModel* sub0 = (*sub)[0];
			InternalMachineModel* sub1 = (*sub)[1];

			typename Place::LevelDescription ld;
			ld.total_size = ch->get_size();
			ld.local_id = 0;
			ld.num_partners = sub1->get_size();
			ld.partners = places + offset + sub0->get_size();
			ld.memory_level = ch->get_memory_level();
			levels->push_back(&ld);
			initialize_places(sub0, offset, levels);
			ld.local_id = sub0->get_size();
			ld.num_partners = ld.local_id;
			ld.partners = places + offset;
			ld.memory_level = ch->get_memory_level();
			initialize_places(sub1, offset + ld.local_id, levels);

			levels->pop_back();
		}
		else {
			InternalMachineModel* sub0 = (*sub)[0];

			initialize_places(sub0, offset, levels);
		}
	}
	else {
		typename Place::LevelDescription ld;
		ld.total_size = 1;
		ld.local_id = 0;
		ld.num_partners = 0;
		ld.partners = NULL;
		ld.memory_level = ch->get_memory_level();
		levels->push_back(&ld);
		Place *place = new Place(levels, ch->get_cpus(), &state, performance_counters);
		places[offset] = place;
		levels->pop_back();
	}
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::finish(TaskParams&& ... params) {
	CallTaskType task(static_cast<TaskParams&&>(params) ...);
	state.startup_task = &task;
	state.current_state = 1;

	// signal scheduler places that execution may start
	state.state_barrier.signal(0);

	for(procs_t i = 0; i < num_places; i++) {
		places[i]->join();
	}

	for(procs_t i = 0; i < num_places; i++) {
		delete places[i];
	}
	delete[] places;
}
*/
template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::print_name() {
	std::cout << name << "<";
	Place::TaskStorage::print_name();
	std::cout << ", " << (int)CallThreshold << ">";
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::print_performance_counter_values() {
	performance_counters.print_values();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::print_performance_counter_headers() {
	performance_counters.print_headers();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
typename PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place() {
	return Place::get();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
procs_t PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_id() {
	return Place::get()->get_id();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class Scheduler> class DefaultStrategyT, uint8_t CallThreshold>
typename PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* PriorityScheduler<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_at(procs_t place_id) {
	assert(place_id < num_places);
	return places[place_id];
}

}


#endif /* PRIORITYSCHEDULER_H_ */
