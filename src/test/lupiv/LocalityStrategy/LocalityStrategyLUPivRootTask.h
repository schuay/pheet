/*
 * LocalityStrategyLUPivRootTask.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LOCALITYSTRATEGYLUPIVROOTTASK_H_
#define LOCALITYSTRATEGYLUPIVROOTTASK_H_

#include "LocalityStrategyLUPivPivotTask.h"
#include "LocalityStrategyLUPivStandardPathTask.h"
#include "LocalityStrategyLUPivCriticalPathTask.h"
#include "LUPivLocalityStrategy.h"

#include <algorithm>

extern "C" {
void dgetrf_(int *m, int *n, double *a, int *lda, int *piv, int *info);
void dgetf2_(int *m, int *n, double *a, int *lda, int *piv, int *info);
}

namespace pheet {

template <class Task, int BLOCK_SIZE = 4>
class LocalityStrategyLUPivRootTask : public Task {
public:
	LocalityStrategyLUPivRootTask(double* a, int* pivot, int m, int lda, int n);
	~LocalityStrategyLUPivRootTask();

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
LocalityStrategyLUPivRootTask<Task, BLOCK_SIZE>::LocalityStrategyLUPivRootTask(double* a, int* pivot, int m, int lda, int n)
: a(a), pivot(pivot), m(m), lda(lda), n(n) {
	assert(m > 0);
	assert(n > 0);
	assert(lda >= m);
	// TODO: debug cases n != m  - until then:
	assert(m == n);
}

template <class Task, int BLOCK_SIZE>
LocalityStrategyLUPivRootTask<Task, BLOCK_SIZE>::~LocalityStrategyLUPivRootTask() {

}

template <class Task, int BLOCK_SIZE>
void LocalityStrategyLUPivRootTask<Task, BLOCK_SIZE>::operator()(typename Task::TEC& tec) {
	int num_blocks = std::min(n, m) / BLOCK_SIZE;

	// Run sequential algorithm on first column
	int out = 0;
	int tmp = std::min(BLOCK_SIZE, n);
	dgetf2_(&m, &tmp, a, &lda, pivot, &out);

	if(num_blocks > 1) {
		double* cur_a = a;
		int* cur_piv = pivot;
		int cur_m = m;

		typename Task::TEC** column_owners = new typename Task::TEC*[num_blocks];
		typename Task::TEC** block_owners = new typename Task::TEC*[num_blocks*num_blocks];
		for(int i = 0; i < num_blocks; ++i) {
			column_owners[i] = &tec;
			for(int j = 0; j < num_blocks; ++j) {
				block_owners[j + i*num_blocks] = &tec;
			}
		}
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
					spawn_prio<LocalityStrategyLUPivCriticalPathTask<Task, BLOCK_SIZE> >(
							LUPivLocalityStrategy<typename Task::Scheduler>(column_owners[i], 3, 5),
							column_owners + i, block_owners + i + (i*num_blocks),
							cur_a + i*BLOCK_SIZE*lda, cur_a + (i-1)*BLOCK_SIZE*lda, cur_piv, cur_m, lda, (i == num_blocks - 1)?(n - BLOCK_SIZE*i):(BLOCK_SIZE));

				// Workflow for other unfinished columns
				for(int j = i + 1; j < num_blocks; ++j) {
					tec.template
						spawn_prio<LocalityStrategyLUPivStandardPathTask<Task, BLOCK_SIZE> >(
								LUPivLocalityStrategy<typename Task::Scheduler>(column_owners[j], 2, 4),
								column_owners + j, block_owners + i + (j*num_blocks),
								cur_a + j*BLOCK_SIZE*lda, cur_a + (i-1)*BLOCK_SIZE*lda, cur_piv, cur_m, lda, (j == num_blocks - 1)?(n - BLOCK_SIZE*j):(BLOCK_SIZE));
				}

				// Pivoting for all other columns
				for(int j = 0; j < (i-1); ++j) {
					tec.template
						spawn_prio<LocalityStrategyLUPivPivotTask<Task> >(
								LUPivLocalityStrategy<typename Task::Scheduler>(column_owners[j], 1, 1),
								column_owners + j,
								cur_a + j*BLOCK_SIZE*lda, cur_piv, std::min(cur_m, BLOCK_SIZE), lda, BLOCK_SIZE);
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
					spawn_prio<LocalityStrategyLUPivPivotTask<Task> >(
							LUPivLocalityStrategy<typename Task::Scheduler>(column_owners[j], 1, 1),
							column_owners + j,
							cur_a + j*BLOCK_SIZE*lda, cur_piv, std::min(cur_m, BLOCK_SIZE), lda, BLOCK_SIZE);
			}
		}

		delete[] column_owners;
		delete[] block_owners;

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

#endif /* LOCALITYSTRATEGYLUPIVROOTTASK_H_ */
