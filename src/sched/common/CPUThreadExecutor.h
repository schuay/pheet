/*
 * CPUThreadExecutor.h
 *
 *  Created on: 13.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef CPUTHREADEXECUTOR_H_
#define CPUTHREADEXECUTOR_H_

#include <pthread.h>

namespace pheet {

template <class Executor>
void* execute_cpu_thread(void *param);

template <class CPUDescriptor, class T>
class CPUThreadExecutor {
public:
	CPUThreadExecutor(vector<CPUDescriptor*>* cpus, T* work);
	~CPUThreadExecutor();

	void run();
	void join();

private:
	void execute();

	CPUDescriptor** desc;
	procs_t num_cpus;
	T* work;
	pthread_t thread;

	template <class Executor>
	friend void* execute_cpu_thread(Executor *param);
};

template <class CPUDescriptor, class T>
CPUThreadExecutor<CPUDescriptor, T>::CPUThreadExecutor(vector<CPUDescriptor*>* cpus, T* work)
: num_cpus(cpus.size()), work(work) {
	this->cpus = new CPUDescriptor*[num_cpus];

	for(size_t i = 0; i < num_cpus; i++) {
		this->cpus[i] = cpus[i];
	}
}

template <class CPUDescriptor, class T>
CPUThreadExecutor<CPUDescriptor, T>::~CPUThreadExecutor() {
	delete[] cpus;
}

template <class CPUDescriptor, class T>
void CPUThreadExecutor<CPUDescriptor, T>::run() {
	pthread_create(&thread, NULL, execute_cpu_thread, this);
}

template <class CPUDescriptor, class T>
void CPUThreadExecutor<CPUDescriptor, T>::join() {
	pthread_join(thread, NULL);
}

template <class CPUDescriptor, class T>
void CPUThreadExecutor<CPUDescriptor, T>::execute() {
	int err;

	#ifdef ENV_LINUX_GCC
	cpu_set_t cpu_affinity;
	CPU_ZERO(&cpu_affinity);
	for(size_t i = 0; i < num_cpus; i++) {
		CPU_SET(cpus[i]->get_cpu_id(), &cpu_affinity);
	}
	if((err = pthread_setaffinity_np(pthread_self(), sizeof(cpu_affinity),
			&cpu_affinity)) != 0)
	{
		/*
		switch(err)
		{
		case EFAULT:
			cout << "EFAULT" << endl;
			break;
		case EINVAL:
			cout << "EINVAL" << endl;
			break;
		case EPERM:
			cout << "EPERM" << endl;
			break;
		case ESRCH:
			cout << "ESRCH" << endl;
			break;
		}
		*/
		cerr << "Failed to bind cpu thread to cpu " << desc->get_cpu_id() << endl;
	}
/*	#else
	#ifdef ENV_SOLARIS_SUNCC
	if((err = processor_bind(P_LWPID, P_MYID, 1, NULL)) != 0)
	{
		cerr << "Failed to bind cpu thread to cpu " << desc->get_cpu_id() << endl;
	}

	#endif
	#endif*/

	work->run();
}

template <class Executor>
void *execute_cpu_thread(void *param) {
	Executor* exec = (Executor*) param;
	exec->run();
}

}

#endif /* CPUTHREADEXECUTOR_H_ */
