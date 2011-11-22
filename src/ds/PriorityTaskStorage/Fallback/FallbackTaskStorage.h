/*
 * FallbackTaskStorage.h
 *
 *  Created on: 21.09.2011
 *      Author: mwimmer
 */

#ifndef FALLBACKTASKSTORAGE_H_
#define FALLBACKTASKSTORAGE_H_

#include "../../../misc/type_traits.h"
#include "../../../misc/atomics.h"

#include <limits>
#include <iostream>

namespace pheet {

template <typename TT, template <typename S> class BaseType>
class FallbackTaskStorage {
public:
	typedef TT T;
	typedef struct{
		void print_headers() {}
		void print_values() {}
	} PerformanceCounters;

	FallbackTaskStorage(size_t expected_capacity);
//	FallbackTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count);
	~FallbackTaskStorage();

	template <class Strategy>
	void push(Strategy s, T item);
	template <class Strategy>
	void push(Strategy s, T item, PerformanceCounters& pc);
	T pop();
	T pop(PerformanceCounters& pc);
	T peek();
	T peek(PerformanceCounters& pc);
	T steal();
	T steal(PerformanceCounters& pc);

	T steal_push(FallbackTaskStorage<TT, BaseType> &other);
	T steal_push(FallbackTaskStorage<TT, BaseType> &other, PerformanceCounters& pc);

	size_t get_length();
	size_t get_length(PerformanceCounters& pc);
	bool is_empty();
	bool is_empty(PerformanceCounters& pc);
	bool is_full();
	bool is_full(PerformanceCounters& pc);

	static void print_name();

private:
	BaseType<TT> data;

	PerformanceCounters perf_count;
};

template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::FallbackTaskStorage(size_t expected_capacity)
: data(expected_capacity) {

}
/*
template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::FallbackTaskStorage(size_t expected_capacity, PerformanceCounters& perf_count)
: data(expected_capacity), perf_count(perf_count) {

}*/

template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::~FallbackTaskStorage() {

}

template <typename TT, template <typename S> class BaseType>
template <class Strategy>
inline void FallbackTaskStorage<TT, BaseType>::push(Strategy s, T item) {
	data.push(item);
}

template <typename TT, template <typename S> class BaseType>
template <class Strategy>
inline void FallbackTaskStorage<TT, BaseType>::push(Strategy s, T item, PerformanceCounters& pc) {
	data.push(item);
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::pop() {
	return data.pop();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::pop(PerformanceCounters& pc) {
	return data.pop();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::peek() {
	return data.peek();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::peek(PerformanceCounters& pc) {
	return data.peek();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal() {
	return data.steal();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal(PerformanceCounters& pc) {
	return data.steal();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal_push(FallbackTaskStorage<TT, BaseType>& other) {
	return data.steal_push(other.data);
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal_push(FallbackTaskStorage<TT, BaseType>& other, PerformanceCounters& pc) {
	return data.steal_push(other.data);
}

template <typename TT, template <typename S> class BaseType>
inline size_t FallbackTaskStorage<TT, BaseType>::get_length() {
	return data.get_length();
}

template <typename TT, template <typename S> class BaseType>
inline size_t FallbackTaskStorage<TT, BaseType>::get_length(PerformanceCounters& pc) {
	return data.get_length();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_empty() {
	return data.is_empty();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_empty(PerformanceCounters& pc) {
	return data.is_empty();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_full() {
	return data.is_full();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_full(PerformanceCounters& pc) {
	return data.is_full();
}

template <typename TT, template <typename S> class BaseType>
void FallbackTaskStorage<TT, BaseType>::print_name() {
	std::cout << "FallbackTaskStorage";
}

}

#endif /* FALLBACKTASKSTORAGE_H_ */
