/*
 * LocalityStrategyLUPivStandardPathTask.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LOCALITYSTRATEGYLUPIVSTANDARDPATHTASK_H_
#define LOCALITYSTRATEGYLUPIVSTANDARDPATHTASK_H_

#include "../helpers/LUPivPivotTask.h"
#include "LocalityStrategyLUPivMMTask.h"
#include "LUPivLocalityStrategy.h"

#include <algorithm>

extern "C" {
void dtrsm_(char *side, char *uplo, char* transa, char* diag, int *m, int *n, double* alpha, double* a, int* lda, double* b, int* ldb);
}

namespace pheet {

template <class Task, int BLOCK_SIZE>
class LocalityStrategyLUPivStandardPathTask : public Task {
public:
	LocalityStrategyLUPivStandardPathTask(typename Task::TEC** owner_info, typename Task::TEC** block_owners, double* a, double* lu_col, int* pivot, int m, int lda, int n);
	virtual ~LocalityStrategyLUPivStandardPathTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	typename Task::TEC** owner_info;
	typename Task::TEC** block_owners;
	double* a;
	double* lu_col;
	int* pivot;
	int m;
	int lda;
	int n;
};

template <class Task, int BLOCK_SIZE>
LocalityStrategyLUPivStandardPathTask<Task, BLOCK_SIZE>::LocalityStrategyLUPivStandardPathTask(typename Task::TEC** owner_info, typename Task::TEC** block_owners, double* a, double* lu_col, int* pivot, int m, int lda, int n)
: owner_info(owner_info), block_owners(block_owners), a(a), lu_col(lu_col), pivot(pivot), m(m), lda(lda), n(n) {

}

template <class Task, int BLOCK_SIZE>
LocalityStrategyLUPivStandardPathTask<Task, BLOCK_SIZE>::~LocalityStrategyLUPivStandardPathTask() {

}

template <class Task, int BLOCK_SIZE>
void LocalityStrategyLUPivStandardPathTask<Task, BLOCK_SIZE>::operator()(typename Task::TEC& tec) {
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

	// Apply matrix multiplication to all other blocks
	int num_blocks = m / BLOCK_SIZE;

	for(int i = 1; i < num_blocks; ++i) {
		int pos = i * BLOCK_SIZE;
		tec.template
			spawn_prio<LocalityStrategyLUPivMMTask<Task> >(
					LUPivLocalityStrategy<typename Task::Scheduler>(block_owners[i], 4, 2),
					block_owners + i,
					lu_col + pos, a, a + pos, k, lda);
	}
}

}

#endif /* LOCALITYSTRATEGYLUPIVSTANDARDPATHTASK_H_ */
