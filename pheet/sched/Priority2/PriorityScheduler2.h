/*
 * PriorityScheduler2.h
 *
 *  Created on: Mar 7, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2_H_
#define PRIORITYSCHEDULER2_H_

namespace pheet {

template <class Pheet>
struct PriorityScheduler2State {
	PriorityScheduler2State();

	uint8_t current_state;
	typename Pheet::Barrier state_barrier;
	typename Pheet::Scheduler::Task* startup_task;
};

template <class Pheet>
PriorityScheduler2State<Pheet>::PrioritySchedulerState()
: current_state(0), startup_task(NULL) {

}


template <class Pheet>
struct PriorityScheduler2TaskStorageItem {
	typedef typename Pheet::Place Place;

	PriorityScheduler2TaskStorageItem();

	typename Pheet::Place::Task* task;
	typename Pheet::Place::StackElement* stack_element;

	bool operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
	bool operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
};

template <class Pheet>
PriorityScheduler2TaskStorageItem<Pheet>::PriorityScheduler2TaskStorageItem()
: task(NULL), stack_element(NULL) {

}

template <class Pheet>
bool PriorityScheduler2TaskStorageItem<Pheet>::operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task == task;
}

template <class Pheet>
bool PriorityScheduler2TaskStorageItem<Pheet>::operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task != task;
}


template <class Pheet>
class nullable_traits<PriorityScheduler2TaskStorageItem<Pheet> > {
public:
	static PriorityScheduler2TaskStorageItem<Pheet> const null_value;
};

template <class Pheet>
PriorityScheduler2TaskStorageItem<Pheet> const nullable_traits<PriorityScheduler2TaskStorageItem<Pheet> >::null_value;


template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P, class TS> class StealerT, template <class P> class DefaultStrategyT>
class PriorityScheduler2Impl {
public:
	typedef typename Pheet::Backoff Backoff;
	typedef typename Pheet::MachineModel MachineModel;
	typedef BinaryTreeMachineModel<Pheet, MachineModel> InternalMachineModel;
	typedef PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold> Self;
	typedef SchedulerTask<Pheet> Task;
	template <typename F>
	using FunctorTask = SchedulerFunctorTask<Pheet, F>;
	typedef PriorityScheduler2TaskStorageItem<Pheet> TaskStorageItem;
	typedef TaskStorageT<Pheet, TaskStorageItem> TaskStorage;
	typedef StealerT<Pheet, TaskStorage> Stealer;
	typedef PriorityScheduler2Place<Pheet, CallThreshold> Place;
	typedef PriorityScheduler2State<Pheet> State;
	typedef FinishRegion<Pheet> Finish;
	typedef PriorityScheduler2PlaceDescriptor<Pheet> PlaceDesc;
	typedef PriorityScheduler2TaskDescriptor<Pheet> TaskDesc;
	typedef DefaultStrategyT<Pheet> DefaultStrategy;
	typedef typename Place::PerformanceCounters PerformanceCounters;

	/*
	 * Uses complete machine
	 */
	PriorityScheduler2Impl();
	PriorityScheduler2Impl(PerformanceCounters& performance_counters);

	/*
	 * Only uses the given number of places
	 * (Currently no direct support for oversubscription)
	 */
	PriorityScheduler2Impl(procs_t num_places);
	PriorityScheduler2Impl(procs_t num_places, PerformanceCounters& performance_counters);
	~PriorityScheduler2Impl();

	static void print_name();

//	static void print_performance_counter_values(Place::PerformanceCounters performance_counters);

//	static void print_performance_counter_headers();

	static Place* get_place();
	static procs_t get_place_id();
	Place* get_place_at(procs_t place_id);

	template<class CallTaskType, typename ... TaskParams>
		void finish(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		void finish(F&& f, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void call(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		void call(F&& f, TaskParams&& ... params);

	template<class CallTaskType, typename ... TaskParams>
		void spawn(TaskParams&& ... params);

	template<typename F, typename ... TaskParams>
		void spawn(F&& f, TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		void spawn_s(Strategy s, TaskParams&& ... params);

	template<class Strategy, typename F, typename ... TaskParams>
		void spawn_s(Strategy s, F&& f, TaskParams&& ... params);

	template<class CallTaskType, class Strategy, typename ... TaskParams>
		void spawn_s(Strategy&& s, TaskParams&& ... params);

	template<class Strategy, typename F, typename ... TaskParams>
		void spawn_s(Strategy&& s, F&& f, TaskParams&& ... params);

	static char const name[];
	static procs_t const max_cpus;

private:
	InternalMachineModel machine_model;
	Place** places;
	procs_t num_places;

	State state;

	PerformanceCounters performance_counters;
};


template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
char const PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::name[] = "PriorityScheduler";

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
procs_t const PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler2Impl()
: num_places(machine_model.get_num_leaves()) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler2Impl(typename Place::PerformanceCounters& performance_counters)
: num_places(machine_model.get_num_leaves()) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler2Impl(procs_t num_places)
: num_places(num_places) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::PriorityScheduler2Impl(procs_t num_places, typename Place::PerformanceCounters& performance_counters)
: num_places(num_places) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::~PriorityScheduler2Impl() {
	delete places[0];
	delete[] places;
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::print_name() {
	std::cout << name << "<";
	Place::TaskStorage::print_name();
	std::cout << ", " << (int)CallThreshold << ">";
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
typename PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place() {
	return Place::get();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
procs_t PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_id() {
	return Place::get()->get_id();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
typename PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_at(procs_t place_id) {
	pheet_assert(place_id < num_places);
	return places[place_id];
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::finish(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->finish<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::finish(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->finish(f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn(f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, class Strategy, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy s, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s<CallTaskType>(std::move(s), std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class Strategy, typename F, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy s, F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s(std::move(s), f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, class Strategy, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy&& s, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s<CallTaskType>(s, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class Strategy, typename F, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy&& s, F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s(s, f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::call(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->call<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void PriorityScheduler2Impl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::call(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->call(f, std::forward<TaskParams&&>(params) ...);
}

/*
template<class Pheet>
using PriorityScheduler2 = PriorityScheduler2Impl<Pheet, ModularTaskStorage, LifoFifoStrategy, 3>;
*/
}

#endif /* PRIORITYSCHEDULER2_H_ */
