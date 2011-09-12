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

template <typename T, bool> class MaxPerformanceCounter;

template <typename T>
class MaxPerformanceCounter<T, false> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter const& other);
	~MaxPerformanceCounter();

	void add_value();
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

inline
template <typename T>
MaxPerformanceCounter<T, false>::MaxPerformanceCounter() {

}

template <typename T>
inline
MaxPerformanceCounter<T, false>::MaxPerformanceCounter(MaxPerformanceCounter const& other) {

}

template <typename T>
inline
MaxPerformanceCounter<T, false>::~MaxPerformanceCounter() {

}

template <typename T>
inline
void MaxPerformanceCounter<T, false>::incr() {

}

template <typename T>
inline
void MaxPerformanceCounter<T, false>::print(char const* const formatting_string) {

}

template <typename T>
inline
void MaxPerformanceCounter<T, false>::print_header(char const* const string) {

}

template <typename T>
class MaxPerformanceCounter<T, true> {
public:
	MaxPerformanceCounter();
	MaxPerformanceCounter(MaxPerformanceCounter<true> const& other);
	~MaxPerformanceCounter();

	void incr();
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
private:
	MaxReducer<size_t> reducer;
};

template <typename T>
inline
MaxPerformanceCounter<T, true>::MaxPerformanceCounter() {

}

template <typename T>
inline
MaxPerformanceCounter<T, true>::MaxPerformanceCounter(MaxPerformanceCounter<true> const& other)
: reducer(other.reducer) {

}

template <typename T>
inline
MaxPerformanceCounter<T, true>::~MaxPerformanceCounter() {

}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::incr() {
	reducer.incr();
}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_sum());
}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::print_header(char const* const string) {
	cout << string;
}

}

#endif /* MAXPERFORMANCECOUNTER_H_ */
