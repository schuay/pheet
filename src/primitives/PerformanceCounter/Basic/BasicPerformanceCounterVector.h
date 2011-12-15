/*
 * BasicPerformanceCounterVector.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICPERFORMANCECOUNTERVECTOR_H_
#define BASICPERFORMANCECOUNTERVECTOR_H_

#include <stdio.h>
#include <iostream>

#include "../../../settings.h"
#include "../../Reducer/Sum/VectorSumReducer.h"

/*
 *
 */
namespace pheet {

template <class Scheduler, bool> class BasicPerformanceCounterVector;

template <class Scheduler>
class BasicPerformanceCounterVector<Scheduler, false> {
public:
	BasicPerformanceCounterVector(size_t length);
	BasicPerformanceCounterVector(BasicPerformanceCounterVector<Scheduler, false>& other);
	~BasicPerformanceCounterVector();

	void incr(size_t i);
	void print(size_t i, char const* const formatting_string);
	void print_header(size_t i, char const* const string);

//	size_t get_length();
};

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, false>::BasicPerformanceCounterVector(size_t length) {

}

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, false>::BasicPerformanceCounterVector(BasicPerformanceCounterVector<Scheduler, false>& other) {

}

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, false>::~BasicPerformanceCounterVector() {

}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, false>::incr(size_t i) {

}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, false>::print(size_t i, char const* const formatting_string) {

}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, false>::print_header(size_t i, char const* const string) {

}
/*
size_t BasicPerformanceCounterVector<Scheduler, false>::get_length() {
	return 0;
}*/

template <class Scheduler>
class BasicPerformanceCounterVector<Scheduler, true> {
public:
	BasicPerformanceCounterVector(size_t length);
	BasicPerformanceCounterVector(BasicPerformanceCounterVector<Scheduler, true> & other);
	~BasicPerformanceCounterVector();

	void incr(size_t i);
	void print(size_t i, char const* const formatting_string);
	void print_header(char const* const string);

//	size_t get_length();
private:
	VectorSumReducer<Scheduler, size_t> reducer;
};

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, true>::BasicPerformanceCounterVector(size_t length)
: reducer(length) {

}

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, true>::BasicPerformanceCounterVector(BasicPerformanceCounterVector<Scheduler, true>& other)
: reducer(other.reducer) {

}

template <class Scheduler>
inline
BasicPerformanceCounterVector<Scheduler, true>::~BasicPerformanceCounterVector() {

}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, true>::incr(size_t i) {
	reducer.incr(i);
}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, true>::print(size_t i, char const* const formatting_string) {
	size_t const* data = reducer.get_sum();
	printf(formatting_string, data[i]);
}

template <class Scheduler>
inline
void BasicPerformanceCounterVector<Scheduler, true>::print_header(char const* const string) {
	std::cout << string;
}
/*
size_t BasicPerformanceCounterVector<Scheduler, true>::get_length() {
	return reducer.get_length();
}*/

}

#endif /* BASICPERFORMANCECOUNTER_H_ */
