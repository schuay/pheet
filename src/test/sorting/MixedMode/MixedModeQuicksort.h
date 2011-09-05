/*
 * MixedModeQuicksort.h
 *
 *  Created on: 30.07.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef MIXEDMODEQUICKSORT_H_
#define MIXEDMODEQUICKSORT_H_

#include "../../../misc/types.h"
#include "../../../misc/atomics.h"
#include "MixedModeQuicksortTask.h"

namespace pheet {

template <class Scheduler, size_t BLOCK_SIZE = 4096>
class MixedModeQuicksort {
public:
	MixedModeQuicksort(procs_t cpus, unsigned int* data, size_t length);
	~MixedModeQuicksort();

	void sort();
	void print_results();

	void print_headers();

	static procs_t const max_cpus;
	static char const name[];
	static char const * const scheduler_name;

private:
	unsigned int* data;
	size_t length;
	typename Scheduler::CPUHierarchy cpu_hierarchy;
	Scheduler scheduler;
};

template <class Scheduler, size_t BLOCK_SIZE>
procs_t const MixedModeQuicksort<Scheduler, BLOCK_SIZE>::max_cpus = Scheduler::max_cpus;

template <class Scheduler, size_t BLOCK_SIZE>
char const MixedModeQuicksort<Scheduler, BLOCK_SIZE>::name[] = "MixedMode Quicksort";

template <class Scheduler, size_t BLOCK_SIZE>
char const * const MixedModeQuicksort<Scheduler, BLOCK_SIZE>::scheduler_name = Scheduler::name;

template <class Scheduler, size_t BLOCK_SIZE>
MixedModeQuicksort<Scheduler, BLOCK_SIZE>::MixedModeQuicksort(procs_t cpus, unsigned int *data, size_t length)
: data(data), length(length), cpu_hierarchy(cpus), scheduler(&cpu_hierarchy) {

}

template <class Scheduler, size_t BLOCK_SIZE>
MixedModeQuicksort<Scheduler, BLOCK_SIZE>::~MixedModeQuicksort() {

}

template <class Scheduler, size_t BLOCK_SIZE>
void MixedModeQuicksort<Scheduler, BLOCK_SIZE>::sort() {
	scheduler.template finish_nt<MixedModeQuicksortTask<typename Scheduler::Task, BLOCK_SIZE> >(((length / BLOCK_SIZE) / 8) + 1, data, length);
}

template <class Scheduler, size_t BLOCK_SIZE>
void MixedModeQuicksort<Scheduler, BLOCK_SIZE>::print_results() {
	scheduler.print_performance_counter_values();
}

template <class Scheduler, size_t BLOCK_SIZE>
void MixedModeQuicksort<Scheduler, BLOCK_SIZE>::print_headers() {
	scheduler.print_performance_counter_headers();
}

}

#endif /* MIXEDMODEQUICKSORT_H_ */
