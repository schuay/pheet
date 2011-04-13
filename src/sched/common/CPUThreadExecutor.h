/*
 * CPUThreadExecutor.h
 *
 *  Created on: 13.04.2011
 *      Author: mwimmer
 */

#ifndef CPUTHREADEXECUTOR_H_
#define CPUTHREADEXECUTOR_H_

#include <pthread.h>

namespace pheet {

template <class Executor>
void *execute_cpu_thread(void *param);

template <class HWDescriptor, class T>
class CPUThreadExecutor {
public:
	CPUThreadExecutor(HWDescriptor *desc, T* work);
	~CPUThreadExecutor();

	void run();
	void join();

private:
	void execute();

	HWDescriptor* desc;
	T* work;
	pthread_t thread;

	template <class Executor>
	friend void *execute_cpu_thread(Executor *param);
};

template <class HWDescriptor, class T>
CPUThreadExecutor<HWDescriptor, T>::CPUThreadExecutor(HWDescriptor *desc, T* work)
: desc(desc), work(work) {

}

template <class HWDescriptor, class T>
CPUThreadExecutor<HWDescriptor, T>::~CPUThreadExecutor() {

}

template <class HWDescriptor, class T>
void CPUThreadExecutor<HWDescriptor, T>::run() {
	pthread_create(&thread, NULL, executeSchedulerThread, this);
}

template <class HWDescriptor, class T>
void CPUThreadExecutor<HWDescriptor, T>::join() {
	pthread_join(thread, NULL);
}

template <class HWDescriptor, class T>
void CPUThreadExecutor<HWDescriptor, T>::execute() {
	int err;

	#ifdef ENV_LINUX_GCC
	cpu_set_t cpu_affinity;
	CPU_ZERO(&cpu_affinity);
	CPU_SET(desc->get_cpu_id(), &cpu_affinity);
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
	#else
	#ifdef ENV_SOLARIS_SUNCC
	if((err = processor_bind(P_LWPID, P_MYID, 1, NULL)) != 0)
	{
		cerr << "Failed to bind cpu thread to cpu " << desc->get_cpu_id() << endl;
	}

	#endif
	#endif

	work->run();
}

}

#endif /* CPUTHREADEXECUTOR_H_ */
