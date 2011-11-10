/*
 * MinPerformanceCounter.h
 *
 *  Created on: Nov 8, 2011
 *      Author: mwimmer
 */

#ifndef MINPERFORMANCECOUNTER_H_
#define MINPERFORMANCECOUNTER_H_

#include <stdio.h>
#include <iostream>

#include "../../../settings.h"
#include "../../Reducer/Min/MinReducer.h"

namespace pheet {

template <typename T, bool> class MinPerformanceCounter;

template <typename T>
class MinPerformanceCounter<T, false> {
public:
	MinPerformanceCounter();
	MinPerformanceCounter(MinPerformanceCounter<T, false> const& other);
	~MinPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <typename T>
inline
MinPerformanceCounter<T, false>::MinPerformanceCounter() {

}

template <typename T>
inline
MinPerformanceCounter<T, false>::MinPerformanceCounter(MinPerformanceCounter<T, false> const& other) {

}

template <typename T>
inline
MinPerformanceCounter<T, false>::~MinPerformanceCounter() {

}

template <typename T>
inline
void MinPerformanceCounter<T, false>::add_value(size_t value) {

}

template <typename T>
inline
void MinPerformanceCounter<T, false>::print(char const* const formatting_string) {

}

template <typename T>
inline
void MinPerformanceCounter<T, false>::print_header(char const* const string) {

}

template <typename T>
class MinPerformanceCounter<T, true> {
public:
	MinPerformanceCounter();
	MinPerformanceCounter(MinPerformanceCounter<T, true>& other);
	~MinPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
private:
	MinReducer<size_t> reducer;
};

template <typename T>
inline
MinPerformanceCounter<T, true>::MinPerformanceCounter() {

}

template <typename T>
inline
MinPerformanceCounter<T, true>::MinPerformanceCounter(MinPerformanceCounter<T, true>& other)
: reducer(other.reducer) {

}

template <typename T>
inline
MinPerformanceCounter<T, true>::~MinPerformanceCounter() {

}

template <typename T>
inline
void MinPerformanceCounter<T, true>::add_value(size_t value) {
	reducer.add_value(value);
}

template <typename T>
inline
void MinPerformanceCounter<T, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_min());
}

template <typename T>
inline
void MinPerformanceCounter<T, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* MINPERFORMANCECOUNTER_H_ */
