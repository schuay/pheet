/*
 * LUPivPivotTask.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVPIVOTTASK_H_
#define LUPIVPIVOTTASK_H_

extern "C" {
void dswap_(int* n, double* sx, int* incx, double* sy, int* incy);
}

namespace pheet {

template <class Task>
class LUPivPivotTask : public Task {
public:
	LUPivPivotTask(double* a, int* pivot, int m, int lda, int n);
	virtual ~LUPivPivotTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	double* a;
	int* pivot;
	int m;
	int lda;
	int n;
};

template <class Task>
LUPivPivotTask<Task>::LUPivPivotTask(double* a, int* pivot, int m, int lda, int n)
: a(a), pivot(pivot), m(m), lda(lda), n(n) {

}

template <class Task>
LUPivPivotTask<Task>::~LUPivPivotTask() {

}

template <class Task>
void LUPivPivotTask<Task>::operator()(typename Task::TEC& tec) {
	for(int j = 0; j < m; ++j) {
		if(pivot[j] != j + 1) {
			dswap_(&n,a + j, &lda, a + pivot[j] - 1, &lda);
		}
	}
}

}

#endif /* LUPIVPIVOTTASK_H_ */
