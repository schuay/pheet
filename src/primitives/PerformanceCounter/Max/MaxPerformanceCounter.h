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
	MaxPerformanceCounter(MaxPerformanceCounter<T, false> const& other);
	~MaxPerformanceCounter();

	void add_value(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <typename T>
inline MaxPerformanceCounter<T, false>::MaxPerformanceCounter() {

}

template <typename T>
inline MaxPerformanceCounter<T, false>::MaxPerformanceCounter(MaxPerformanceCounter<T, false> const& other) {

}

template <typename T>
inline
MaxPerformanceCounter<T, false>::~MaxPerformanceCounter() {

}

template <typename T>
inline
void MaxPerformanceCounter<T, false>::add_value(size_t value) {

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
	MaxPerformanceCounter(MaxPerformanceCounter<T, true>& other);
	~MaxPerformanceCounter();

	void add_value(size_t value);
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
MaxPerformanceCounter<T, true>::MaxPerformanceCounter(MaxPerformanceCounter<T, true>& other)
: reducer(other.reducer) {

}

template <typename T>
inline
MaxPerformanceCounter<T, true>::~MaxPerformanceCounter() {

}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::add_value(size_t value) {
	reducer.add_value(value);
}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_max());
}

template <typename T>
inline
void MaxPerformanceCounter<T, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* MAXPERFORMANCECOUNTER_H_ */
