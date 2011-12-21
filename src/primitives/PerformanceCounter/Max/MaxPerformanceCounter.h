/*
 * MaxPerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef MAXPERFORMANCECOUNTER_H_
#define MAXPERFORMANCECOUNTER_H_

#include <stdio.h>
#include <iostream>

#include "../../../settings.h"
#include "../../Reducer/Max/MaxReducer.h"

/*
 *
 */
namespace pheet {

template <class Scheduler, typename T, bool> class MaxPerformanceCounter;

template <class Scheduler, typename T>
class MaxPerformanceCounter<Scheduler, T, false> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter<Scheduler, T, false> const& other);
	~MaxPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <class Scheduler, typename T>
inline MaxPerformanceCounter<Scheduler, T, false>::MaxPerformanceCounter() {

}

template <class Scheduler, typename T>
inline MaxPerformanceCounter<Scheduler, T, false>::MaxPerformanceCounter(MaxPerformanceCounter<Scheduler, T, false> const& other) {

}

template <class Scheduler, typename T>
inline
MaxPerformanceCounter<Scheduler, T, false>::~MaxPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, false>::add_value(size_t value) {

}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, false>::print(char const* const formatting_string) {

}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, false>::print_header(char const* const string) {

}

template <class Scheduler, typename T>
class MaxPerformanceCounter<Scheduler, T, true> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter<Scheduler, T, true>& other);
	~MaxPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
private:
	MaxReducer<Scheduler, size_t> reducer;
};

template <class Scheduler, typename T>
inline
MaxPerformanceCounter<Scheduler, T, true>::MaxPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
MaxPerformanceCounter<Scheduler, T, true>::MaxPerformanceCounter(MaxPerformanceCounter<Scheduler, T, true>& other)
: reducer(other.reducer) {

}

template <class Scheduler, typename T>
inline
MaxPerformanceCounter<Scheduler, T, true>::~MaxPerformanceCounter() {

}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, true>::add_value(size_t value) {
	reducer.add_value(value);
}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_max());
}

template <class Scheduler, typename T>
inline
void MaxPerformanceCounter<Scheduler, T, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* MAXPERFORMANCECOUNTER_H_ */
