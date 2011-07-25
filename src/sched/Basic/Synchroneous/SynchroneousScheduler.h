/*
 * SynchroneousScheduler.h
 *
 *  Created on: 06.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SYNCHRONEOUSSCHEDULER_H_
#define SYNCHRONEOUSSCHEDULER_H_

#include "../../../../settings.h"
#include "../../common/SchedulerTask.h"
#include "SynchroneousSchedulerTaskExecutionContext.h"

#include <assert.h>
#include <vector>

#ifdef ENV_LINUX_GCC
#include <pthread.h>
#endif

namespace pheet {

template <class CPUHierarchyT>
class SynchroneousScheduler {
public:
	typedef SchedulerTask<SynchroneousScheduler> Task;
	typedef SynchroneousSchedulerTaskExecutionContext<SynchroneousScheduler> TaskExecutionContext;
	typedef CPUHierarchyT CPUHierarchy;

	SynchroneousScheduler(CPUHierarchy* cpus);
	virtual ~SynchroneousScheduler();

	template<class CallTaskType, typename ... TaskParams>
	void finish(TaskParams ... params);

	static char const name[];
	static procs_t const max_cpus;

private:
	TaskExecutionContext tec;
#ifdef ENV_LINUX_GCC
	cpu_set_t old_cpu_affinity;
#endif
};

template <class CPUHierarchyT>
char const SynchroneousScheduler<CPUHierarchyT>::name[] = "SynchroneousScheduler";

template <class CPUHierarchyT>
procs_t const SynchroneousScheduler<CPUHierarchyT>::max_cpus = 1;

template <class CPUHierarchyT>
SynchroneousScheduler<CPUHierarchyT>::SynchroneousScheduler(CPUHierarchy* cpus)
: tec(this){
	assert(cpus->get_size() == 1);

#ifdef ENV_LINUX_GCC
	int err;
	pthread_getaffinity_np(pthread_self(), sizeof(old_cpu_affinity),
				&old_cpu_affinity);
	cpu_set_t cpu_affinity;
	CPU_ZERO(&cpu_affinity);
	vector<typename CPUHierarchy::CPUDescriptor*> const* cpu_descs;
	cpu_descs = cpus->get_cpus();
	for(size_t i = 0; i < cpu_descs->size(); i++) {
		CPU_SET((*cpu_descs)[i]->get_physical_id(), &cpu_affinity);
	}
	if((err = pthread_setaffinity_np(pthread_self(), sizeof(cpu_affinity),
			&cpu_affinity)) != 0)
	{
		cerr << "Failed to bind scheduler thread to cpu(s)" << endl;
	}
#endif
}

template <class CPUHierarchyT>
SynchroneousScheduler<CPUHierarchyT>::~SynchroneousScheduler() {
#ifdef ENV_LINUX_GCC
	int err;
	if((err = pthread_setaffinity_np(pthread_self(), sizeof(old_cpu_affinity),
			&old_cpu_affinity)) != 0)
	{
		cerr << "Failed to reset cpu affinity" << endl;
	}
#endif
}

template <class CPUHierarchyT>
template<class CallTaskType, typename ... TaskParams>
void SynchroneousScheduler<CPUHierarchyT>::finish(TaskParams ... params) {
	CallTaskType task(params ...);
	task(tec);
}

}

#endif /* SYNCHRONEOUSSCHEDULER_H_ */
