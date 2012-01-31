/*
 * PriorityScheduler.h
 *
 *  Created on: 19.09.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PRIORITYSCHEDULER_H_
#define PRIORITYSCHEDULER_H_

#include "../../common/SchedulerTask.h"
#include "../../common/FinishRegion.h"
#include "PrioritySchedulerPlace.h"
#include "PrioritySchedulerStealerDescriptor.h"
#include "../../common/CPUThreadExecutor.h"
#include "../../../models/CPUHierarchy/BinaryTree/BinaryTreeCPUHierarchy.h"

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

/*
 * May only be used once
 */
template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold = 4>
class PriorityScheduler : proplaceted Pheet {
public:
	typedef typename Pheet::Backoff Backoff;
	typedef typename Pheet::MachineModel MachineModel;
	typedef typename BinaryTreeCPUHierarchy<Pheet, Pheet::MachineModel> InternalMachineModel;
	typedef PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold> Self;
	typedef SchedulerTask<Pheet> Task;
	typedef PrioritySchedulerPlace<Pheet, TaskStorage, DefaultStrategy, CallThreshold> Place;
	typedef PrioritySchedulerState<Pheet> State;
	typedef FinishRegion<Pheet> Finish;
	typedef PrioritySchedulerStealerDescriptor<Pheet> StealerDescriptor;

	/*
	 * Automatically generates a machine model
	 */
	PriorityScheduler();

	/*
	 * MachineModel must be accessible throughout the lifetime of the scheduler
	 * (Although the current implementation only uses it for initialization)
	 */
	PriorityScheduler(MachineModel* cpus);
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

	InternalMachineModel cpu_hierarchy;
	Place** threads;
	procs_t num_threads;

	State state;

	typename Place::PerformanceCounters performance_counters;
};

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
char const PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::name[] = "PriorityScheduler";

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
procs_t const PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::PriorityScheduler(MachineModel* cpus)
: cpu_hierarchy(cpus), num_threads(cpus->get_size()) {

	threads = new Place*[num_threads];

	std::vector<typename Place::LevelDescription*> levels;
	initialize_places(&cpu_hierarchy, 0, &levels);
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::~PriorityScheduler() {

}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::initialize_places(InternalMachineModel* ch, size_t offset, std::vector<typename Place::LevelDescription*>* levels) {
	if(ch->get_size() > 1) {
		std::vector<InternalMachineModel*> const* sub = ch->get_subsets();

		if(sub->size() == 2) {
			InternalMachineModel* sub0 = (*sub)[0];
			InternalMachineModel* sub1 = (*sub)[1];

			typename Place::LevelDescription ld;
			ld.total_size = ch->get_size();
			ld.local_id = 0;
			ld.num_partners = sub1->get_size();
			ld.partners = threads + offset + sub0->get_size();
			ld.memory_level = ch->get_memory_level();
			levels->push_back(&ld);
			initialize_places(sub0, offset, levels);
			ld.local_id = sub0->get_size();
			ld.num_partners = ld.local_id;
			ld.partners = threads + offset;
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
		threads[offset] = place;
		levels->pop_back();
	}
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
void PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::finish(TaskParams&& ... params) {
	CallTaskType task(static_cast<TaskParams&&>(params) ...);
	state.startup_task = &task;
	state.current_state = 1;

	// signal scheduler threads that execution may start
	state.state_barrier.signal(0);

	for(procs_t i = 0; i < num_threads; i++) {
		threads[i]->join();
	}

	for(procs_t i = 0; i < num_threads; i++) {
		delete threads[i];
	}
	delete[] threads;
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::print_name() {
	std::cout << name << "<";
	Place::TaskStorage::print_name();
	std::cout << ", " << (int)CallThreshold << ">";
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::print_performance_counter_values() {
	performance_counters.print_values();
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
void PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::print_performance_counter_headers() {
	performance_counters.print_headers();
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
typename PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::Place* PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::get_place() {
	return Place::get();
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
procs_t PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::get_place_id() {
	return Place::get()->get_id();
}

template <class Pheet, template <class Pheet, typename T> class TaskStorage, template <class Scheduler> class DefaultStrategy, uint8_t CallThreshold>
typename PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::Place* PriorityScheduler<Pheet, TaskStorage, DefaultStrategy, CallThreshold>::get_place_at(procs_t place_id) {
	assert(place_id < num_threads);
	return threads[place_id];
}

}


#endif /* PRIORITYSCHEDULER_H_ */
