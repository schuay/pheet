/*
 * MixedModeForkJoinQuicksort.h
 *
 *  Created on: 08.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef MIXEDMODEFORKJOINQUICKSORT_H_
#define MIXEDMODEFORKJOINQUICKSORT_H_

#include "../../../misc/types.h"
#include "../../../misc/atomics.h"
#include "MixedModeForkJoinQuicksortTask.h"
//#include "../../../sched/MixedMode/Synchroneous/SynchroneousMixedModeScheduler.h"
//#include "../../../sched/MixedMode/Synchroneous/SynchroneousMixedModeTask.h"

namespace pheet {

template <class Scheduler>
class MixedModeForkJoinQuicksort {
public:
	MixedModeForkJoinQuicksort(procs_t cpus, unsigned int* data, size_t length);
	~MixedModeForkJoinQuicksort();

	void sort();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	unsigned int* data;
	size_t length;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
};

template <class Scheduler>
procs_t const MixedModeForkJoinQuicksort<Scheduler>::max_cpus = Scheduler::max_cpus;

template <class Scheduler>
char const MixedModeForkJoinQuicksort<Scheduler>::name[] = "MixedMode Fork-Join Quicksort";

template <class Scheduler>
char const * const MixedModeForkJoinQuicksort<Scheduler>::scheduler_name = Scheduler::name;

template <class Scheduler>
MixedModeForkJoinQuicksort<Scheduler>::MixedModeForkJoinQuicksort(procs_t cpus, unsigned int *data, size_t length)
: data(data), length(length), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

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
