/*
 * MixedModeForkJoinQuicksort.h
 *
 *  Created on: 08.04.2011
 *      Author: mwimmer
 */

#ifndef MIXEDMODEFORKJOINQUICKSORT_H_
#define MIXEDMODEFORKJOINQUICKSORT_H_

#include "../../../misc/types.h"
#include "MixedModeForkJoinQuicksortTask.h"
//#include "../../../sched/MixedMode/Synchroneous/SynchroneousMixedModeScheduler.h"
//#include "../../../sched/MixedMode/Synchroneous/SynchroneousMixedModeTask.h"

namespace pheet {

template <class Scheduler>
class MixedModeForkJoinQuicksort {
public:
	MixedModeForkJoinQuicksort(unsigned int* data, size_t length);
	~MixedModeForkJoinQuicksort();

	void sort();

	static const procs_t max_cpus;
	static const char name[];

private:
	unsigned int* data;
	size_t length;
	Scheduler scheduler;
};

template <class Scheduler>
const procs_t MixedModeForkJoinQuicksort<Scheduler>::max_cpus = 1;

template <class Scheduler>
const char MixedModeForkJoinQuicksort<Scheduler>::name[] = "MixedMode Fork-Join Quicksort";

template <class Scheduler>
MixedModeForkJoinQuicksort<Scheduler>::MixedModeForkJoinQuicksort(unsigned int *data, size_t length)
: data(data), length(length), scheduler() {

}

template <class Scheduler>
MixedModeForkJoinQuicksort<Scheduler>::~MixedModeForkJoinQuicksort() {

}

template <class Scheduler>
void MixedModeForkJoinQuicksort<Scheduler>::sort() {
	scheduler.template finish<MixedModeForkJoinQuicksortTask<typename Scheduler::Task> >(data, length);
}

}

#endif /* MIXEDMODEFORKJOINQUICKSORT_H_ */
