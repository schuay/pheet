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

	FallbackTaskStorage(size_t initial_capacity);
	FallbackTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count);
	~FallbackTaskStorage();

	template <class Strategy>
	void push(Strategy s, T item);
	T pop();
	T peek();
	T steal();

	T steal_push(FallbackTaskStorage<TT, BaseType> &other);

	size_t get_length();
	bool is_empty();
	bool is_full();

	static void print_name();

private:
	BaseType<TT> data;

	PerformanceCounters perf_count;
};

template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::FallbackTaskStorage(size_t initial_capacity)
: data(initial_capacity) {

}

template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::FallbackTaskStorage(size_t initial_capacity, PerformanceCounters& perf_count)
: data(initial_capacity), perf_count(perf_count) {

}

template <typename TT, template <typename S> class BaseType>
inline FallbackTaskStorage<TT, BaseType>::~FallbackTaskStorage() {

}

template <typename TT, template <typename S> class BaseType>
template <class Strategy>
inline void FallbackTaskStorage<TT, BaseType>::push(Strategy s, T item) {
	data.push(item);
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::pop() {
	return data.pop();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::peek() {
	return data.peek();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal() {
	return data.steal();
}

template <typename TT, template <typename S> class BaseType>
inline TT FallbackTaskStorage<TT, BaseType>::steal_push(FallbackTaskStorage<TT, BaseType>& other) {
	return data.steal_push(other.data);
}

template <typename TT, template <typename S> class BaseType>
inline size_t FallbackTaskStorage<TT, BaseType>::get_length() {
	return data.get_length();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_empty() {
	return data.is_empty();
}

template <typename TT, template <typename S> class BaseType>
inline bool FallbackTaskStorage<TT, BaseType>::is_full() {
	return data.is_full();
}

template <typename TT, template <typename S> class BaseType>
void FallbackTaskStorage<TT, BaseType>::print_name() {
	std::cout << "FallbackTaskStorage";
}

}

#endif /* FALLBACKTASKSTORAGE_H_ */
