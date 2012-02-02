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

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
class ModularTaskStorage {
public:
	typedef ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT> Self;
	typedef TT T;
	typedef typename Pheet::Scheduler Scheduler;
	typedef PrimaryT<Scheduler, T> Primary;
	typedef SecondaryT<Scheduler, T, PrimaryT> Secondary;
	typedef typename Pheet::Scheduler::StealerDescriptor StealerDescriptor;
	typedef ModularTaskStoragePerformanceCounters<Scheduler, typename Primary::PerformanceCounters, typename Secondary::PerformanceCounters> PerformanceCounters;

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
	T steal(StealerDescriptor& sd);
	T steal(StealerDescriptor& sd, PerformanceCounters& pc);

	T steal_push(Self& other, StealerDescriptor& sd);
	T steal_push(Self& other, StealerDescriptor& sd, PerformanceCounters& pc);

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
	Primary primary;
	// The destructor if the secondary structure is called first (C++ standard) - That's exactly what we want
	// (Destruction happens in reverse order of construction)
	Secondary secondary;

	PerformanceCounters perf_count;
};

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::ModularTaskStorage(size_t expected_capacity)
: primary(expected_capacity), secondary(&primary, expected_capacity) {

}
/*
template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::ModularTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: primary(initial_capacity, perf_count.primary_perf_count), secondary(&primary, perf_count.secondary_perf_count), perf_count(perf_count) {

}*/

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::~ModularTaskStorage() {

}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
template <class Strategy>
inline void ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::push(Strategy& s, T item) {
	PerformanceCounters pc;
	primary.push(s, item, pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
template <class Strategy>
inline void ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::push(Strategy& s, T item, PerformanceCounters& pc) {
	primary.push(s, item, pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::pop() {
	PerformanceCounters pc;
	return primary.pop(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::pop(PerformanceCounters& pc) {
	return primary.pop(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::peek() {
	PerformanceCounters pc;
	return primary.peek(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::peek(PerformanceCounters& pc) {
	return primary.peek(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::steal(StealerDescriptor& sd) {
	PerformanceCounters pc;
	return secondary.steal(sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::steal(StealerDescriptor& sd, PerformanceCounters& pc) {
	return secondary.steal(sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::steal_push(ModularTaskStorage& other, StealerDescriptor& sd) {
	// Currently we don't plan to support stealing more than one task, as this would require require reconfiguring the strategies
	PerformanceCounters pc;
	return secondary.steal_push(other.primary, sd, pc.secondary_perf_count, pc.secondary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline TT ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::steal_push(Self& other, StealerDescriptor& sd, PerformanceCounters& pc) {
	// Currently we don't plan to support stealing more than one task, as this would require require reconfiguring the strategies
	return secondary.steal_push(other.primary, sd, pc.primary_perf_count, pc.secondary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline size_t ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::get_length() {
	PerformanceCounters pc;
	return primary.get_length(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline size_t ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::get_length(PerformanceCounters& pc) {
	return primary.get_length(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline bool ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::is_empty() {
	PerformanceCounters pc;
	return primary.is_empty(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline bool ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::is_empty(PerformanceCounters& pc) {
	return primary.is_empty(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline bool ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::is_full() {
	PerformanceCounters pc;
	return primary.is_full(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline bool ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::is_full(PerformanceCounters& pc) {
	return primary.is_full(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
inline void ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::perform_maintenance(PerformanceCounters& pc) {
	primary.perform_maintenance(pc.primary_perf_count);
}

template <class Pheet, typename TT, template <class Scheduler, typename S> class PrimaryT, template <class Scheduler, typename S, template <class S, typename Q> class P> class SecondaryT>
void ModularTaskStorage<Pheet, TT, PrimaryT, SecondaryT>::print_name() {
	std::cout << "ModularTaskStorage<";
	Primary::print_name();
	std::cout << ", ";
	Secondary::print_name();
	std::cout << ">";
}

}

#endif /* MODULARTASKSTORAGE_H_ */
