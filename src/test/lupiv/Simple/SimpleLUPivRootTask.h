/*
 * SimpleLUPivRootTask.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef SIMPLELUPIVROOTTASK_H_
#define SIMPLELUPIVROOTTASK_H_

#include "../helpers/LUPivPivotTask.h"
#include "SimpleLUPivStandardPathTask.h"
#include "SimpleLUPivCriticalPathTask.h"

#include <algorithm>

extern "C" {
void dgetrf_(int *m, int *n, double *a, int *lda, int *piv, int *info);
void dgetf2_(int *m, int *n, double *a, int *lda, int *piv, int *info);
}

namespace pheet {

template <class Task, int BLOCK_SIZE = 128>
class SimpleLUPivRootTask : public Task {
public:
	SimpleLUPivRootTask(double* a, int* pivot, int m, int lda, int n);
	~SimpleLUPivRootTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	// The matrix (column-major)
	double* a;
	// vector containing the pivot indices for the rows (length: m)
	int* pivot;
	// Number of rows in a
	int m;
	// Leading dimension (lda >= max(1, m))
	int lda;
	// Number of columns in a
	int n;
};

template <class Task, int BLOCK_SIZE>
SimpleLUPivRootTask<Task, BLOCK_SIZE>::SimpleLUPivRootTask(double* a, int* pivot, int m, int lda, int n)
: a(a), pivot(pivot), m(m), lda(lda), n(n) {
	assert(m > 0);
	assert(n > 0);
	assert(lda >= m);
	// TODO: debug cases n != m  - until then:
	assert(m == n);
}

template <class Task, int BLOCK_SIZE>
SimpleLUPivRootTask<Task, BLOCK_SIZE>::~SimpleLUPivRootTask() {

}

template <class Task, int BLOCK_SIZE>
void SimpleLUPivRootTask<Task, BLOCK_SIZE>::operator()(typename Task::TEC& tec) {
	int num_blocks = std::min(n, m) / BLOCK_SIZE;

	// Run sequential algorithm on first column
	int out = 0;
	int tmp = std::min(BLOCK_SIZE, n);
	dgetf2_(&m, &tmp, a, &lda, pivot, &out);

	if(num_blocks > 1) {
		double* cur_a = a;
		int* cur_piv = pivot;
		int cur_m = m;
		/*
		 * Algorithm by blocks. For each iteration perform this work
		 * Could be improved to be even more finegrained with a dag, where some dgemm tasks can still be performed
		 * while we already execute the next iteration
		 */
		for(int i = 1; i < num_blocks; ++i) {
			// Finish one iteration before you can start the next one
			{typename Task::Finish f(tec);
				// Critical path
				tec.template
					spawn<SimpleLUPivCriticalPathTask<Task, BLOCK_SIZE> >(cur_a + i*BLOCK_SIZE*lda, cur_a + (i-1)*BLOCK_SIZE*lda, cur_piv, cur_m, lda, (i == num_blocks - 1)?(n - BLOCK_SIZE*i):(BLOCK_SIZE));

				// Workflow for other unfinished columns
				for(int j = i + 1; j < num_blocks; ++j) {
					tec.template
						spawn<SimpleLUPivStandardPathTask<Task, BLOCK_SIZE> >(cur_a + j*BLOCK_SIZE*lda, cur_a + (i-1)*BLOCK_SIZE*lda, cur_piv, cur_m, lda, (j == num_blocks - 1)?(n - BLOCK_SIZE*j):(BLOCK_SIZE));
				}

				// Pivoting for all other columns
				for(int j = 0; j < (i-1); ++j) {
					tec.template
						spawn<LUPivPivotTask<Task> >(cur_a + j*BLOCK_SIZE*lda, cur_piv, std::min(cur_m, BLOCK_SIZE), lda, BLOCK_SIZE);
				}

				cur_a += BLOCK_SIZE;
				cur_piv += BLOCK_SIZE;
				cur_m -= BLOCK_SIZE;
			}
		}
		{typename Task::Finish f(tec);
			// Pivoting for all other columns
			for(int j = 0; j < (num_blocks-1); ++j) {
				tec.template
					spawn<LUPivPivotTask<Task> >(cur_a + j*BLOCK_SIZE*lda, cur_piv, std::min(cur_m, BLOCK_SIZE), lda, BLOCK_SIZE);
			}
		}

		// Update pivots as the offsets are calculated from the beginning of the block
		for(int i = BLOCK_SIZE; i < m; i += BLOCK_SIZE) {
			for(int j = i; j < i+BLOCK_SIZE; j++) {
				assert(pivot[j] != 0);
				assert(pivot[j] <= m-i);
				pivot[j] = pivot[j] + i;
				assert(pivot[j] >= j+1);
			}
		}
	}
}

}

#endif /* SIMPLELUPIVROOTTASK_H_ */
