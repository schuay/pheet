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

 class TaskSched
 {
   struct timeval start_time;
   struct timeval stop_time;
   size_t id;
 public:
 TaskSched(struct timeval& start_time, struct timeval& stop_time, size_t id):start_time(start_time),stop_time(stop_time),id(id)
   {}

   TaskSched() {}

   void print()
   {
     std::cout << id << ": " << start_time.tv_sec << " " << stop_time.tv_sec << endl;
   }

 };


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
	ListReducer<Pheet,std::vector<TaskSched>, TaskSched > red;
};


template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters()
{

}

template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters(SORPerformanceCounters<Pheet>& other)
  :slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place),red(other.red)
{

}

template <class Pheet>
inline SORPerformanceCounters<Pheet>::SORPerformanceCounters(SORPerformanceCounters<Pheet>&& other)
  :slices_rescheduled_at_same_place(other.slices_rescheduled_at_same_place),red(other.red)
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


	std::vector<TaskSched> tasks = red.get_list();

	std::cout << "Tasks count " << tasks.size() << endl;

	for(size_t i=0; i<tasks.size(); i++)
	  {
	    tasks[i].print();
	  }
	
}

}



#endif /* SORPERFORMANCECOUNTERS_H_ */
