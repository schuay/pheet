/*
 * SORPerformanceCounters.h
 *
 *  Created on: 21 jun 2012
 *      Author: Daniel Cederman
 */

#ifndef SORPERFORMANCECOUNTERS_H_
#define SORPERFORMANCECOUNTERS_H_

#include "pheet/pheet.h"
#include "pheet/primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"

namespace pheet {

template <class Pheet>
class SORPerformanceCounters {
public:
	SORPerformanceCounters();
	SORPerformanceCounters(SORPerformanceCounters& other);
	SORPerformanceCounters(SORPerformanceCounters&& other);
	~SORPerformanceCounters();

	static void print_headers();
	void print_values();

	BasicPerformanceCounter<Pheet, sor_slices_rescheduled_at_same_place> slices_rescheduled_at_same_place;
};


template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters()
{

}

template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters(SORPerformanceCounters<Pheet>& other)
:slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place)
{

}

template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters(SORPerformanceCounters<Pheet>&& other)
:slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place)
{

}

template <class Pheet>
inline SORPerformanceCounters<Pheet>::~SORPerformanceCounters() {

}

template <class Pheet>
inline void SORPerformanceCounters<Pheet>::print_headers() {
	BasicPerformanceCounter<Pheet, sor_slices_rescheduled_at_same_place>::print_header("slices_rescheduled_at_same_place\t");

}

template <class Pheet>
inline void SORPerformanceCounters<Pheet>::print_values() {
	slices_rescheduled_at_same_place.print("%d\t");
}

}



#endif /* SORPERFORMANCECOUNTERS_H_ */
