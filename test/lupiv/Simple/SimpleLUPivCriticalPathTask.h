/*
 * SimpleLUPivCriticalPathTask.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef SIMPLELUPIVCRITICALPATHTASK_H_
#define SIMPLELUPIVCRITICALPATHTASK_H_

#include "../helpers/LUPivPivotTask.h"
#include "../helpers/LUPivMMTask.h"

#include <algorithm>

extern "C" {
void dtrsm_(char *side, char *uplo, char* transa, char* diag, int *m, int *n, double* alpha, double* a, int* lda, double* b, int* ldb);
void dgetf2_(int *m, int *n, double *a, int *lda, int *piv, int *info);
}

namespace pheet {

template <class Task, int BLOCK_SIZE>
class SimpleLUPivCriticalPathTask : public Task {
public:
	SimpleLUPivCriticalPathTask(double* a, double* lu_col, int* pivot, int m, int lda, int n);
	virtual ~SimpleLUPivCriticalPathTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	double* a;
	double* lu_col;
	int* pivot;
	int m;
	int lda;
	int n;
};

template <class Task, int BLOCK_SIZE>
SimpleLUPivCriticalPathTask<Task, BLOCK_SIZE>::SimpleLUPivCriticalPathTask(double* a, double* lu_col, int* pivot, int m, int lda, int n)
: a(a), lu_col(lu_col), pivot(pivot), m(m), lda(lda), n(n) {

}

template <class Task, int BLOCK_SIZE>
SimpleLUPivCriticalPathTask<Task, BLOCK_SIZE>::~SimpleLUPivCriticalPathTask() {

}

template <class Task, int BLOCK_SIZE>
void SimpleLUPivCriticalPathTask<Task, BLOCK_SIZE>::operator()(typename Task::TEC& tec) {
	assert(n <= BLOCK_SIZE);

	// Apply pivot to column
	tec.template
		call<LUPivPivotTask<Task> >(a, pivot, std::min(m, BLOCK_SIZE), lda, n);

	int k = std::min(std::min(m, n), BLOCK_SIZE);

	{	// Apply TRSM to uppermost block
		char side = 'l';
		char uplo = 'l';
		char transa = 'n';
		char diag = 'u';
		double alpha = 1.0;
		dtrsm_(&side, &uplo, &transa, &diag, &k, &k, &alpha, lu_col, &lda, a, &lda);
	}

	int num_blocks = m / BLOCK_SIZE;

	{typename Task::Finish f(tec);
		// Apply matrix multiplication to all other blocks
		for(int i = 1; i < num_blocks; ++i) {
			int pos = i * BLOCK_SIZE;
			tec.template
				spawn<LUPivMMTask<Task> >(lu_col + pos, a, a + pos, k, lda);
		}
	}

	if(num_blocks > 1) {
		// Apply sequential algorithm to the rest of the column
		int out = 0;
		int tmp_m = m - BLOCK_SIZE;
		int tmp_n = std::min(BLOCK_SIZE, n);
		dgetf2_(&tmp_m, &tmp_n, a + BLOCK_SIZE, &lda, pivot + BLOCK_SIZE, &out);
	}
}

}

#endif /* SIMPLELUPIVCRITICALPATHTASK_H_ */
