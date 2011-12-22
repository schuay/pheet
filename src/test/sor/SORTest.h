/*
* SORTests.h
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Pheet license
*/

#ifndef SORTEST_H_
#define SORTEST_H_

#include "../../settings.h"
#include "../../misc/types.h"
#include "../Test.h"

#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

namespace pheet {

template <class Impl>
class SORTest : Test {
public:
	SORTest(procs_t cpus, int M, int N, int slices, double omega, int iterations);
	~SORTest();

	void run_test();

private:
	procs_t cpus;
	int M;
	int N;
	int slices;
	double omega;
	int iterations;
};

template <class Impl>
SORTest<Impl>::SORTest(procs_t cpus, int M, int N, int slices, double omega, int iterations)
: cpus(cpus),M(M),N(N),slices(slices),omega(omega),iterations(iterations){

}

template <class Impl>
SORTest<Impl>::~SORTest() {

}

template <class Impl>
void SORTest<Impl>::run_test() {

	Impl iar(cpus, M, N, slices, omega, iterations);

	Time start, end;
	check_time(start);
	iar.run();
	check_time(end);

	double seconds = calculate_seconds(start, end);
	std::cout << "test\timplementation\tscheduler\tcpus\ttotal_time\tM\tN\tslices\tomega\titerations\tresult\t";
	iar.print_headers();
	std::cout << std::endl;
	std::cout << "sor\t";
	Impl::print_name();
	std::cout << "\t";
	Impl::print_scheduler_name();
	std::cout << "\t" << cpus << "\t" << seconds << "\t";
	std::cout << M << "\t" << N << "\t" << slices << "\t" << omega << "\t" << iterations << "\t" << iar.getTotal() << "\t";
	iar.print_results();
	std::cout << std::endl;
}
}

#endif /* SORTESTS_H_ */
