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

template <bool> class BasicPerformanceCounterVector;

template <>
class BasicPerformanceCounterVector<false> {
public:
	BasicPerformanceCounterVector(size_t length);
	BasicPerformanceCounterVector(BasicPerformanceCounterVector<false> const& other);
	~BasicPerformanceCounterVector();

	void incr(size_t i);
	void print(size_t i, char const* const formatting_string);
	void print_header(size_t i, char const* const string);

	size_t get_length();
};

BasicPerformanceCounterVector<false>::BasicPerformanceCounterVector(size_t length) {

}

BasicPerformanceCounterVector<false>::BasicPerformanceCounterVector(BasicPerformanceCounterVector<false> const& other) {

}

BasicPerformanceCounterVector<false>::~BasicPerformanceCounterVector() {

}

void BasicPerformanceCounterVector<false>::incr(size_t i) {

}

void BasicPerformanceCounterVector<false>::print(size_t i, char const* const formatting_string) {

}

void BasicPerformanceCounterVector<false>::print_header(size_t i, char const* const string) {

}

size_t BasicPerformanceCounterVector<false>::get_length() {
	return 0;
}

template <>
class BasicPerformanceCounterVector<true> {
public:
	BasicPerformanceCounterVector(size_t length);
	BasicPerformanceCounterVector(BasicPerformanceCounterVector<true> const& other);
	~BasicPerformanceCounterVector();

	void incr(size_t i);
	void print(size_t i, char const* const formatting_string);
	void print_header(char const* const string);

	size_t get_length();
private:
	VectorSumReducer<size_t> reducer;
};

BasicPerformanceCounterVector<true>::BasicPerformanceCounterVector(size_t length)
: reducer(length) {

}

BasicPerformanceCounterVector<true>::BasicPerformanceCounterVector(BasicPerformanceCounterVector<true> const& other)
: reducer(other.reducer) {

}

BasicPerformanceCounterVector<true>::~BasicPerformanceCounterVector() {

}

void BasicPerformanceCounterVector<true>::incr(size_t i) {
	reducer.incr(i);
}

void BasicPerformanceCounterVector<true>::print(size_t i, char const* const formatting_string) {
	size_t const* data = reducer.get_sum();
	printf(formatting_string, data[i]);
}

void BasicPerformanceCounterVector<true>::print_header(char const* const string) {
	cout << string;
}

size_t BasicPerformanceCounterVector<false>::get_length() {
	return reducer.get_length();
}

}

#endif /* BASICPERFORMANCECOUNTER_H_ */
