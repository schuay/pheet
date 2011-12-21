/*
 * QuicksortTest.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SORTINGTESTS_H_
#define SORTINGTESTS_H_

#include "../Test.h"
#include "../../test_settings.h"
#include "SortingTest.h"

namespace pheet {

template <bool ACTIVE> class SortingTests;

template <>
class SortingTests<false> : Test {
public:
	SortingTests() {}
	virtual ~SortingTests() {}

	void run_test() {}

private:
	template<class Sorter>
	void run_sorter() {}
};

template <>
class SortingTests<true> : Test {
public:
	SortingTests();
	virtual ~SortingTests();

	void run_test();

private:
	template<class Sorter>
	void run_sorter();
};

template <class Sorter>
void SortingTests<true>::run_sorter() {
//	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttotal_time\truns\t";
//	Sorter::print_headers();
//	std::cout << std::endl;

	for(size_t t = 0; t < sizeof(sorting_test_types)/sizeof(sorting_test_types[0]); t++) {
		for(size_t n = 0; n < sizeof(sorting_test_n)/sizeof(sorting_test_n[0]); n++) {
			for(size_t c = 0; c < sizeof(sorting_test_cpus)/sizeof(sorting_test_cpus[0]); c++) {
				if(sorting_test_cpus[c] <= Sorter::max_cpus) {
					for(size_t s = 0; s < sizeof(sorting_test_seeds)/sizeof(sorting_test_seeds[0]); s++) {
						SortingTest<Sorter> st(sorting_test_cpus[c], sorting_test_types[t], sorting_test_n[n], sorting_test_seeds[s]);
						st.run_test();
					}
				}
			}
		}
	}
}

}

#endif /* SORTINGTESTS_H_ */
