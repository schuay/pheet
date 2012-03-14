/*
 * StrategyScheduler.h
 *
 *  Created on: Mar 7, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2_H_
#define PRIORITYSCHEDULER2_H_

namespace pheet {

template <class Pheet>
struct StrategySchedulerState {
	StrategySchedulerState();

	uint8_t current_state;
	typename Pheet::Barrier state_barrier;
	typename Pheet::Scheduler::Task* startup_task;
};

template <class Pheet>
StrategySchedulerState<Pheet>::PrioritySchedulerState()
: current_state(0), startup_task(NULL) {

}


template <class Pheet>
struct StrategySchedulerTaskStorageItem {
	typedef typename Pheet::Place Place;

	StrategySchedulerTaskStorageItem();

	typename Pheet::Place::Task* task;
	typename Pheet::Place::StackElement* stack_element;

	bool operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
	bool operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const;
};

template <class Pheet>
StrategySchedulerTaskStorageItem<Pheet>::StrategySchedulerTaskStorageItem()
: task(NULL), stack_element(NULL) {

}

template <class Pheet>
bool StrategySchedulerTaskStorageItem<Pheet>::operator==(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task == task;
}

template <class Pheet>
bool StrategySchedulerTaskStorageItem<Pheet>::operator!=(PrioritySchedulerTaskStorageItem<Pheet> const& other) const {
	return other.task != task;
}


template <class Pheet>
class nullable_traits<StrategySchedulerTaskStorageItem<Pheet> > {
public:
	static StrategySchedulerTaskStorageItem<Pheet> const null_value;
};

template <class Pheet>
StrategySchedulerTaskStorageItem<Pheet> const nullable_traits<StrategySchedulerTaskStorageItem<Pheet> >::null_value;


template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P, class TS> class StealerT, template <class P> class DefaultStrategyT>
class StrategySchedulerImpl {
public:
	typedef typename Pheet::Backoff Backoff;
	typedef typename Pheet::MachineModel MachineModel;
	typedef BinaryTreeMachineModel<Pheet, MachineModel> InternalMachineModel;
	typedef StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold> Self;
	typedef SchedulerTask<Pheet> Task;
	template <typename F>
	using FunctorTask = SchedulerFunctorTask<Pheet, F>;
	typedef StrategySchedulerTaskStorageItem<Pheet> TaskStorageItem;
	typedef TaskStorageT<Pheet, TaskStorageItem> TaskStorage;
	typedef StealerT<Pheet, TaskStorage> Stealer;
	typedef StrategySchedulerPlace<Pheet, CallThreshold> Place;
	typedef StrategySchedulerState<Pheet> State;
	typedef FinishRegion<Pheet> Finish;
	typedef StrategySchedulerPlaceDescriptor<Pheet> PlaceDesc;
	typedef StrategySchedulerTaskDescriptor<Pheet> TaskDesc;
	typedef DefaultStrategyT<Pheet> DefaultStrategy;
	typedef typename Place::PerformanceCounters PerformanceCounters;

	/*
	 * Uses complete machine
	 */
	StrategySchedulerImpl();
	StrategySchedulerImpl(PerformanceCounters& performance_counters);

	/*
	 * Only uses the given number of places
	 * (Currently no direct support for oversubscription)
	 */
	StrategySchedulerImpl(procs_t num_places);
	StrategySchedulerImpl(procs_t num_places, PerformanceCounters& performance_counters);
	~StrategySchedulerImpl();

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
char const StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::name[] = "PriorityScheduler";

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
procs_t const StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::max_cpus = std::numeric_limits<procs_t>::max() >> 1;

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::StrategySchedulerImpl()
: num_places(machine_model.get_num_leaves()) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::StrategySchedulerImpl(typename Place::PerformanceCounters& performance_counters)
: num_places(machine_model.get_num_leaves()) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::StrategySchedulerImpl(procs_t num_places)
: num_places(num_places) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::StrategySchedulerImpl(procs_t num_places, typename Place::PerformanceCounters& performance_counters)
: num_places(num_places) {

	places = new Place*[num_places];
	places[0] = new Place(machine_model, places, num_places, &state, performance_counters);
	places[0]->prepare_root();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::~StrategySchedulerImpl() {
	delete places[0];
	delete[] places;
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::print_name() {
	std::cout << name << "<";
	Place::TaskStorage::print_name();
	std::cout << ", " << (int)CallThreshold << ">";
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
typename StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place() {
	return Place::get();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
procs_t StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_id() {
	return Place::get()->get_id();
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
typename StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::Place* StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::get_place_at(procs_t place_id) {
	pheet_assert(place_id < num_places);
	return places[place_id];
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::finish(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->finish<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::finish(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->finish(f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn(f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, class Strategy, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy s, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s<CallTaskType>(std::move(s), std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class Strategy, typename F, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy s, F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s(std::move(s), f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, class Strategy, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy&& s, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s<CallTaskType>(s, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class Strategy, typename F, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::spawn_s(Strategy&& s, F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->spawn_s(s, f, std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<class CallTaskType, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::call(TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->call<CallTaskType>(std::forward<TaskParams&&>(params) ...);
}

template <class Pheet, template <class P, typename T> class TaskStorageT, template <class P> class DefaultStrategyT, uint8_t CallThreshold>
template<typename F, typename ... TaskParams>
inline void StrategySchedulerImpl<Pheet, TaskStorageT, DefaultStrategyT, CallThreshold>::call(F&& f, TaskParams&& ... params) {
	Place* p = get_place();
	pheet_assert(p != NULL);
	p->call(f, std::forward<TaskParams&&>(params) ...);
}

/*
template<class Pheet>
using StrategyScheduler = StrategySchedulerImpl<Pheet, ModularTaskStorage, LifoFifoStrategy, 3>;
*/
}

#endif /* PRIORITYSCHEDULER2_H_ */
