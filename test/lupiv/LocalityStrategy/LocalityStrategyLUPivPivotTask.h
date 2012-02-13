/*
 * LocalityStrategyLUPivPivotTask.h
 *
 *  Created on: Jan 4, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LOCALITYSTRATEGYLUPIVPIVOTTASK_H_
#define LOCALITYSTRATEGYLUPIVPIVOTTASK_H_

#include "../../../settings.h"

extern "C" {
void dswap_(int* n, double* sx, int* incx, double* sy, int* incy);
}

namespace pheet {

template <class Task>
class LocalityStrategyLUPivPivotTask : public Task {
public:
	LocalityStrategyLUPivPivotTask(typename Task::TEC** owner_info, double* a, int* pivot, int m, int lda, int n);
	virtual ~LocalityStrategyLUPivPivotTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	typename Task::TEC** owner_info;
	double* a;
	int* pivot;
	int m;
	int lda;
	int n;
};

template <class Task>
LocalityStrategyLUPivPivotTask<Task>::LocalityStrategyLUPivPivotTask(typename Task::TEC** owner_info, double* a, int* pivot, int m, int lda, int n)
: owner_info(owner_info), a(a), pivot(pivot), m(m), lda(lda), n(n) {

}

template <class Task>
LocalityStrategyLUPivPivotTask<Task>::~LocalityStrategyLUPivPivotTask() {

}

template <class Task>
void LocalityStrategyLUPivPivotTask<Task>::operator()(typename Task::TEC& tec) {
	(*owner_info) = &tec;

	for(int j = 0; j < m; ++j) {
		if(pivot[j] != j + 1) {
			dswap_(&n,a + j, &lda, a + pivot[j] - 1, &lda);
		}
	}
}

}

#endif /* LOCALITYSTRATEGYLUPIVPIVOTTASK_H_ */
