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

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
class ModularTaskStorage {
public:
	typedef TT T;
	typedef ModularTaskStoragePerformanceCounters<typename Primary<T>::PerformanceCounters, typename Secondary<T, Primary>::PerformanceCounters> PerformanceCounters;

	ModularTaskStorage(size_t initial_capacity);
	ModularTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count);
	~ModularTaskStorage();

	template <class Strategy>
	void push(Strategy& s, T item);
	T pop();
	T peek();
	T steal();

	T steal_push(ModularTaskStorage<TT, Primary, Secondary> &other);

	size_t get_length();
	bool is_empty();
	bool is_full();

	static void print_name();

private:
	Primary<T> primary;
	Secondary<T, Primary> secondary;

	PerformanceCounters perf_count;
};

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
ModularTaskStorage<TT, Primary, Secondary>::ModularTaskStorage(size_t initial_capacity)
: primary(initial_capacity), secondary(&primary) {

}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
ModularTaskStorage<TT, Primary, Secondary>::ModularTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: primary(initial_capacity, perf_count.primary_perf_count), secondary(&primary, perf_count.secondary_perf_count), perf_count(perf_count) {

}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
ModularTaskStorage<TT, Primary, Secondary>::~ModularTaskStorage() {

}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
template <class Strategy>
inline void ModularTaskStorage<TT, Primary, Secondary>::push(Strategy& s, T item) {
	primary.push(s, item);
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline TT ModularTaskStorage<TT, Primary, Secondary>::pop() {
	return primary.pop();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline TT ModularTaskStorage<TT, Primary, Secondary>::peek() {
	return primary.peek();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline TT ModularTaskStorage<TT, Primary, Secondary>::steal() {
	return secondary.steal();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline TT ModularTaskStorage<TT, Primary, Secondary>::steal_push(ModularTaskStorage<TT, Primary, Secondary>& other) {
	// Currently we don't plan to support stealing more than one task, as this would require require reconfiguring the strategies
	return secondary.steal();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline size_t ModularTaskStorage<TT, Primary, Secondary>::get_length() {
	return primary.get_length();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline bool ModularTaskStorage<TT, Primary, Secondary>::is_empty() {
	return primary.is_empty();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
inline bool ModularTaskStorage<TT, Primary, Secondary>::is_full() {
	return primary.is_full();
}

template <typename TT, template <typename S> class Primary, template <typename S, template <typename Q> class P> class Secondary>
void ModularTaskStorage<TT, Primary, Secondary>::print_name() {
	std::cout << "ModularTaskStorage<";
	Primary<T>::print_name();
	std::cout << ", ";
	Secondary<T, Primary>::print_name();
	std::cout << ">";
}

}

#endif /* MODULARTASKSTORAGE_H_ */
