/*
 * PPoPPLUPivPerformanceCounters.h
 *
 *  Created on: 21 jun 2012
 *      Author: Daniel Cederman
 */

#ifndef PPoPPLUPivPerformanceCounters_H_
#define PPoPPLUPivPerformanceCounters_H_

#include "pheet/pheet.h"
#include "pheet/primitives/PerformanceCounter/Basic/BasicPerformanceCounter.h"

namespace pheet {

template <class Pheet>
class PPoPPLUPivPerformanceCounters {
public:
	PPoPPLUPivPerformanceCounters();
	PPoPPLUPivPerformanceCounters(PPoPPLUPivPerformanceCounters& other);
	PPoPPLUPivPerformanceCounters(PPoPPLUPivPerformanceCounters&& other);
	~PPoPPLUPivPerformanceCounters();

	static void print_headers();
	void print_values();

	BasicPerformanceCounter<Pheet, lupiv_slices_rescheduled_at_same_place> slices_rescheduled_at_same_place;
	BasicPerformanceCounter<Pheet, lupiv_total_distance_to_last_location> total_distance_to_last_location;
	BasicPerformanceCounter<Pheet, lupiv_total_tasks> total_tasks;
};


template <class Pheet>
inline PPoPPLUPivPerformanceCounters<Pheet>::PPoPPLUPivPerformanceCounters()
{

}

template <class Pheet>
inline PPoPPLUPivPerformanceCounters<Pheet>::PPoPPLUPivPerformanceCounters(PPoPPLUPivPerformanceCounters<Pheet>& other)
  :slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place),
   total_distance_to_last_location(other.total_distance_to_last_location),
   total_tasks(other.total_tasks)
{

}

template <class Pheet>
inline PPoPPLUPivPerformanceCounters<Pheet>::PPoPPLUPivPerformanceCounters(PPoPPLUPivPerformanceCounters<Pheet>&& other)
  :slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place),
   total_distance_to_last_location(other.total_distance_to_last_location),
   total_tasks(other.total_tasks)

{

}

template <class Pheet>
inline PPoPPLUPivPerformanceCounters<Pheet>::~PPoPPLUPivPerformanceCounters() {

}

template <class Pheet>
inline void PPoPPLUPivPerformanceCounters<Pheet>::print_headers() {

	BasicPerformanceCounter<Pheet, lupiv_slices_rescheduled_at_same_place>::print_header("slices_rescheduled_at_same_place\t");
	BasicPerformanceCounter<Pheet, lupiv_total_distance_to_last_location>::print_header("total_distance_to_last_location\t");
	BasicPerformanceCounter<Pheet, lupiv_total_tasks>::print_header("total_tasks\t");
}

template <class Pheet>
inline void PPoPPLUPivPerformanceCounters<Pheet>::print_values() {
	slices_rescheduled_at_same_place.print("%d\t");
	total_distance_to_last_location.print("%d\t");
	total_tasks.print("%d\t");

	}

}



#endif /* PPoPPLUPivPerformanceCounters_H_ */
