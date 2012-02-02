/*
 * ModularTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef MODULARTASKSTORAGE_H_
#define MODULARTASKSTORAGE_H_

#include <iostream>
#include "ModularTaskStoragePerformanceCounters.h"

namespace pheet {

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
class ModularTaskStorage {
public:
	typedef TT T;
	typedef ModularTaskStoragePerformanceCounters<Scheduler, typename Primary<Scheduler, T>::PerformanceCounters, typename Secondary<Scheduler, T, Primary>::PerformanceCounters> PerformanceCounters;

	ModularTaskStorage(size_t expected_capacity);
//	ModularTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count);
	~ModularTaskStorage();

	template <class Strategy>
	void push(Strategy& s, T item);
	template <class Strategy>
	void push(Strategy& s, T item, PerformanceCounters& pc);
	T pop();
	T pop(PerformanceCounters& pc);
	T peek();
	T peek(PerformanceCounters& pc);
	T steal(typename Scheduler::StealerDescriptor& sd);
	T steal(typename Scheduler::StealerDescriptor& sd, PerformanceCounters& pc);

	T steal_push(ModularTaskStorage<Scheduler, TT, Primary, Secondary> &other, typename Scheduler::StealerDescriptor& sd);
	T steal_push(ModularTaskStorage<Scheduler, TT, Primary, Secondary> &other, typename Scheduler::StealerDescriptor& sd, PerformanceCounters& pc);

	size_t get_length();
	size_t get_length(PerformanceCounters& pc);
	bool is_empty();
	bool is_empty(PerformanceCounters& pc);
	bool is_full();
	bool is_full(PerformanceCounters& pc);

	// Can be called by the scheduler every time it is idle to perform some routine maintenance
	void perform_maintenance(PerformanceCounters& pc);

	static void print_name();

private:
	Primary<Scheduler, T> primary;
	// The destructor if the secondary structure is called first (C++ standard) - That's exactly what we want
	// (Destruction happens in reverse order of construction)
	Secondary<Scheduler, T, Primary> secondary;

	PerformanceCounters perf_count;
};

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
ModularTaskStorage<Scheduler, TT, Primary, Secondary>::ModularTaskStorage(size_t expected_capacity)
: primary(expected_capacity), secondary(&primary, expected_capacity) {

}
/*
template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
ModularTaskStorage<Scheduler, TT, Primary, Secondary>::ModularTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: primary(initial_capacity, perf_count.primary_perf_count), secondary(&primary, perf_count.secondary_perf_count), perf_count(perf_count) {

}*/

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
ModularTaskStorage<Scheduler, TT, Primary, Secondary>::~ModularTaskStorage() {

}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
template <class Strategy>
inline void ModularTaskStorage<Scheduler, TT, Primary, Secondary>::push(Strategy& s, T item) {
	PerformanceCounters pc;
	primary.push(s, item, pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
template <class Strategy>
inline void ModularTaskStorage<Scheduler, TT, Primary, Secondary>::push(Strategy& s, T item, PerformanceCounters& pc) {
	primary.push(s, item, pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::pop() {
	PerformanceCounters pc;
	return primary.pop(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::pop(PerformanceCounters& pc) {
	return primary.pop(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::peek() {
	PerformanceCounters pc;
	return primary.peek(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::peek(PerformanceCounters& pc) {
	return primary.peek(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::steal(typename Scheduler::StealerDescriptor& sd) {
	PerformanceCounters pc;
	return secondary.steal(sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::steal(typename Scheduler::StealerDescriptor& sd, PerformanceCounters& pc) {
	return secondary.steal(sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::steal_push(ModularTaskStorage<Scheduler, TT, Primary, Secondary>& other, typename Scheduler::StealerDescriptor& sd) {
	// Currently we don't plan to support stealing more than one task, as this would require require reconfiguring the strategies
	PerformanceCounters pc;
	return secondary.steal_push(other.primary, sd, pc.secondary_perf_count, pc.secondary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline TT ModularTaskStorage<Scheduler, TT, Primary, Secondary>::steal_push(ModularTaskStorage<Scheduler, TT, Primary, Secondary>& other, typename Scheduler::StealerDescriptor& sd, PerformanceCounters& pc) {
	// Currently we don't plan to support stealing more than one task, as this would require require reconfiguring the strategies
	return secondary.steal_push(other.primary, sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline size_t ModularTaskStorage<Scheduler, TT, Primary, Secondary>::get_length() {
	PerformanceCounters pc;
	return primary.get_length(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline size_t ModularTaskStorage<Scheduler, TT, Primary, Secondary>::get_length(PerformanceCounters& pc) {
	return primary.get_length(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline bool ModularTaskStorage<Scheduler, TT, Primary, Secondary>::is_empty() {
	PerformanceCounters pc;
	return primary.is_empty(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline bool ModularTaskStorage<Scheduler, TT, Primary, Secondary>::is_empty(PerformanceCounters& pc) {
	return primary.is_empty(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline bool ModularTaskStorage<Scheduler, TT, Primary, Secondary>::is_full() {
	PerformanceCounters pc;
	return primary.is_full(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline bool ModularTaskStorage<Scheduler, TT, Primary, Secondary>::is_full(PerformanceCounters& pc) {
	return primary.is_full(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
inline void ModularTaskStorage<Scheduler, TT, Primary, Secondary>::perform_maintenance(PerformanceCounters& pc) {
	primary.perform_maintenance(pc.primary_perf_count);
}

template <class Scheduler, typename TT, template <class EScheduler, typename S> class Primary, template <class EScheduler, typename S, template <class ES, typename Q> class P> class Secondary>
void ModularTaskStorage<Scheduler, TT, Primary, Secondary>::print_name() {
	std::cout << "ModularTaskStorage<";
	Primary<Scheduler, T>::print_name();
	std::cout << ", ";
	Secondary<Scheduler, T, Primary>::print_name();
	std::cout << ">";
}

}

#endif /* MODULARTASKSTORAGE_H_ */
