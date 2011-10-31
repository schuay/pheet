/*
 * BasicPerformanceCounter.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef BASICPERFORMANCECOUNTER_H_
#define BASICPERFORMANCECOUNTER_H_

#include <stdio.h>
#include <iostream>

#include "../../../settings.h"
#include "../../Reducer/Sum/SumReducer.h"

/*
 *
 */
namespace pheet {

template <bool> class BasicPerformanceCounter;

template <>
class BasicPerformanceCounter<false> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<false>& other);
	~BasicPerformanceCounter();

	void incr();
	void add(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

inline
BasicPerformanceCounter<false>::BasicPerformanceCounter() {

}

inline
BasicPerformanceCounter<false>::BasicPerformanceCounter(BasicPerformanceCounter<false>& other) {

}

inline
BasicPerformanceCounter<false>::~BasicPerformanceCounter() {

}

inline
void BasicPerformanceCounter<false>::incr() {

}

inline
void BasicPerformanceCounter<false>::add(size_t value) {

}

inline
void BasicPerformanceCounter<false>::print(char const* const formatting_string) {

}

inline
void BasicPerformanceCounter<false>::print_header(char const* const string) {

}

template <>
class BasicPerformanceCounter<true> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<true>& other);
	~BasicPerformanceCounter();

	void incr();
	void add(size_t value);
	void print(char const* formatting_string);
	static void print_header(char const* const string);
private:
	SumReducer<size_t> reducer;
};

inline
BasicPerformanceCounter<true>::BasicPerformanceCounter() {

}

inline
BasicPerformanceCounter<true>::BasicPerformanceCounter(BasicPerformanceCounter<true>& other)
: reducer(other.reducer) {

}

inline
BasicPerformanceCounter<true>::~BasicPerformanceCounter() {

}

inline
void BasicPerformanceCounter<true>::incr() {
	reducer.incr();
}

inline
void BasicPerformanceCounter<true>::add(size_t value) {
	reducer.add(value);
}

inline
void BasicPerformanceCounter<true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_sum());
}

inline
void BasicPerformanceCounter<true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* BASICPERFORMANCECOUNTER_H_ */
