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

template <class Scheduler, bool> class BasicPerformanceCounter;

template <class Scheduler>
class BasicPerformanceCounter<Scheduler, false> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<Scheduler, false>& other);
	~BasicPerformanceCounter();

	void incr();
	void add(size_t value);
	void print(char const* const formatting_string);
	static void print_header(char const* const string);
};

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, false>::BasicPerformanceCounter() {

}

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, false>::BasicPerformanceCounter(BasicPerformanceCounter<Scheduler, false>& other) {

}

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, false>::~BasicPerformanceCounter() {

}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, false>::incr() {

}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, false>::add(size_t value) {

}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, false>::print(char const* const formatting_string) {

}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, false>::print_header(char const* const string) {

}

template <class Scheduler>
class BasicPerformanceCounter<Scheduler, true> {
public:
	BasicPerformanceCounter();
	BasicPerformanceCounter(BasicPerformanceCounter<Scheduler, true>& other);
	~BasicPerformanceCounter();

	void incr();
	void add(size_t value);
	void print(char const* formatting_string);
	static void print_header(char const* const string);
private:
	SumReducer<Scheduler, size_t> reducer;
};

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, true>::BasicPerformanceCounter() {

}

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, true>::BasicPerformanceCounter(BasicPerformanceCounter<Scheduler, true>& other)
: reducer(other.reducer) {

}

template <class Scheduler>
inline
BasicPerformanceCounter<Scheduler, true>::~BasicPerformanceCounter() {

}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, true>::incr() {
	reducer.incr();
}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, true>::add(size_t value) {
	reducer.add(value);
}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, true>::print(char const* const formatting_string) {
	printf(formatting_string, reducer.get_sum());
}

template <class Scheduler>
inline
void BasicPerformanceCounter<Scheduler, true>::print_header(char const* const string) {
	std::cout << string;
}

}

#endif /* BASICPERFORMANCECOUNTER_H_ */
