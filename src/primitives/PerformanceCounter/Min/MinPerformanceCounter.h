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

template <class Scheduler, typename T, bool> class MinPerformanceCounter;

template <class Scheduler, typename T>
class MinPerformanceCounter<Scheduler, T, false> {
public:
	MinPerformanceCounter();
	MinPerformanceCounter(MinPerformanceCounter<Scheduler, T, false> const& other);
	~MinPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, false>::MinPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, false>::MinPerformanceCounter(MinPerformanceCounter<Scheduler, T, false> const& other) {

}

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, false>::~MinPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, false>::add_value(size_t value) {

}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, false>::print(char const* const formatting_string) {

}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, false>::print_header(char const* const string) {

}

template <class Scheduler, typename T>
class MinPerformanceCounter<Scheduler, T, true> {
public:
	MinPerformanceCounter();
	MinPerformanceCounter(MinPerformanceCounter<Scheduler, T, true>& other);
	~MinPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
private:
	MinReducer<Scheduler, size_t> reducer;
};

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, true>::MinPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, true>::MinPerformanceCounter(MinPerformanceCounter<Scheduler, T, true>& other)
: reducer(other.reducer) {

}

template <class Scheduler, typename T>
inline
MinPerformanceCounter<Scheduler, T, true>::~MinPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, true>::add_value(size_t value) {
	reducer.add_value(value);
}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_min());
}

template <class Scheduler, typename T>
inline
void MinPerformanceCounter<Scheduler, T, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* MINPERFORMANCECOUNTER_H_ */
