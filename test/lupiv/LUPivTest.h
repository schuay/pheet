/*
 * LUPivTest.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVTEST_H_
#define LUPIVTEST_H_

#include <iostream>

#include <boost/random/uniform_real.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <math.h>

extern "C" {
void dswap_(int* n, double* sx, int* incx, double* sy, int* incy);
void dgemm_(char * transA, char* transB, int* m, int* n, int* k, double* alpha, double* a, int* lda, double* b, int* ldb, double* beta, double* c, int* ldc);
}

namespace pheet {

template <class Kernel>
class LUPivTest : Test {
public:
	LUPivTest(procs_t cpus, int type, size_t size, unsigned int seed);
	virtual ~LUPivTest();

	void run_test();

private:
	double* generate_data();
	double verify(double *matrix, int* pivot);

	procs_t cpus;
	int type;
	size_t size;
	unsigned int seed;

	static char const* const types[];
};

template <class Kernel>
char const* const LUPivTest<Kernel>::types[] = {"random"};

template <class Kernel>
LUPivTest<Kernel>::LUPivTest(procs_t cpus, int type, size_t size, unsigned int seed)
: cpus(cpus), type(type), size(size), seed(seed) {

}

template <class Kernel>
LUPivTest<Kernel>::~LUPivTest() {

}

template <class Kernel>
void LUPivTest<Kernel>::run_test() {
	double* data = generate_data();
	int* pivot = new int[size];

	Kernel s(cpus, data, pivot, size);

	Time start, end;
	check_time(start);
	s.run();
	check_time(end);

	double eps = verify(data, pivot);
	double seconds = calculate_seconds(start, end);
	std::cout << "test\tkernel\tscheduler\ttype\tsize\tseed\tcpus\ttotal_time\teps\t";
	s.print_headers();
	std::cout << std::endl;
	std::cout << "lu_piv\t" << Kernel::name << "\t";
	Kernel::print_scheduler_name();
	std::cout << "\t" << types[type] << "\t" << size << "\t" << seed << "\t" << cpus << "\t" << seconds << "\t" << eps << "\t";
	s.print_results();
	std::cout << std::endl;

	delete[] pivot;
	delete[] data;
}


template <class Kernel>
double* LUPivTest<Kernel>::generate_data() {
	double* data = new double[size*size];

	boost::mt19937 rng;
	rng.seed(seed);

	switch(type) {
	case 0:
		// Random
		{
			boost::uniform_real<double> rnd_st(-1.0, 1.0);

			for(size_t i = 0; i < size*size; i++) {
				data[i] = rnd_st(rng);
			}
		}
		break;
	default:
		throw "unknown type for sorter";
	}

	return data;
}

template <class Kernel>
double LUPivTest<Kernel>::verify(double *matrix, int* pivot) {
	double *origA = generate_data();

	int n = (int)size;
	double *L = new double[n*n];
	double *U = new double[n*n];
	double *A = new double[n*n];
	for(size_t i = 0; i < size; i++) {	// go through the columns
		for(size_t j = 0; j < size; j++) {	// go through the rows
			if(i == j) {
				L[i*n + j] = 1;
				U[i*n + j] = matrix[i*n + j];
			}
			else if(i < j) {
				L[i*n + j] = matrix[i*n + j];
				U[i*n + j] = 0.0;
			}
			else {
				L[i*n + j] = 0.0;
				U[i*n + j] = matrix[i*n + j];
			}
		}
	}

	char trans = 'n';
	double alpha = 1.0;
	double beta = 0.0;

	dgemm_(&trans, &trans, &n, &n, &n, &alpha, L, &n, U, &n, &beta, A, &n);

	for(size_t j = 0; j < size; j++) {
		if(pivot[j] != (int)(j + 1)) {
			dswap_(&n, origA + j, &n, origA + pivot[j] - 1, &n);
		}
	}

	double eps = 0;
	for(size_t i = 0; i < size; i++) {	// go through the columns
		for(size_t j = 0; j < size; j++) {	// go through the rows
			eps += fabs((double)(origA[i*n + j] - A[i*n + j]));
		}
	}
	delete[] origA;
	delete[] A;
	delete[] L;
	delete[] U;

	return eps;
}


}

#endif /* LUPIVTEST_H_ */
