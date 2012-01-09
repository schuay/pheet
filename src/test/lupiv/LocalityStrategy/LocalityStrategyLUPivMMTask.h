/*
 * LocalityStrategyLUPivMMTask.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LOCALITYSTRATEGYLUPIVMMTASK_H_
#define LOCALITYSTRATEGYLUPIVMMTASK_H_

#include "../../../settings.h"

extern "C" {
void dgemm_(char * transA, char* transB, int* m, int* n, int* k, double* alpha, double* a, int* lda, double* b, int* ldb, double* beta, double* c, int* ldc);
}

namespace pheet {

template <class Task>
class LocalityStrategyLUPivMMTask : public Task {
public:
	LocalityStrategyLUPivMMTask(typename Task::TEC** owner_info, double* a, double* b, double* c, int k, int lda);
	virtual ~LocalityStrategyLUPivMMTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	typename Task::TEC** owner_info;
	double* a;
	double* b;
	double* c;
	int k;
	int lda;
};

template <class Task>
LocalityStrategyLUPivMMTask<Task>::LocalityStrategyLUPivMMTask(typename Task::TEC** owner_info, double* a, double* b, double* c, int k, int lda)
: owner_info(owner_info), a(a), b(b), c(c), k(k), lda(lda) {

}

template <class Task>
LocalityStrategyLUPivMMTask<Task>::~LocalityStrategyLUPivMMTask() {

}

template <class Task>
void LocalityStrategyLUPivMMTask<Task>::operator()(typename Task::TEC& tec) {
	(*owner_info) = &tec;

	char trans_a = 'n';
	char trans_b = 'n';
	double alpha = -1.0;
	double beta = 1.0;
	dgemm_(&trans_a, &trans_b, &k, &k, &k, &alpha, a, &lda, b, &lda, &beta, c, &lda);
}

}

#endif /* LOCALITYSTRATEGYLUPIVMMTASK_H_ */
