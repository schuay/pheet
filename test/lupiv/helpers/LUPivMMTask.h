/*
 * LUPivMMTask.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVMMTASK_H_
#define LUPIVMMTASK_H_

#include "../../../settings.h"

extern "C" {
void dgemm_(char * transA, char* transB, int* m, int* n, int* k, double* alpha, double* a, int* lda, double* b, int* ldb, double* beta, double* c, int* ldc);
}

namespace pheet {

template <class Task>
class LUPivMMTask : public Task {
public:
	LUPivMMTask(double* a, double* b, double* c, int k, int lda);
	virtual ~LUPivMMTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	double* a;
	double* b;
	double* c;
	int k;
	int lda;
};

template <class Task>
LUPivMMTask<Task>::LUPivMMTask(double* a, double* b, double* c, int k, int lda)
: a(a), b(b), c(c), k(k), lda(lda) {

}

template <class Task>
LUPivMMTask<Task>::~LUPivMMTask() {

}

template <class Task>
void LUPivMMTask<Task>::operator()(typename Task::TEC& tec) {
	char trans_a = 'n';
	char trans_b = 'n';
	double alpha = -1.0;
	double beta = 1.0;
	dgemm_(&trans_a, &trans_b, &k, &k, &k, &alpha, a, &lda, b, &lda, &beta, c, &lda);
}

}

#endif /* LUPIVMMTASK_H_ */
