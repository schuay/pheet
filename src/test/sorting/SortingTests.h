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
#ifdef SORTING_TEST
#include "SortingTest.h"
#endif

namespace pheet {

class SortingTests : Test {
public:
	SortingTests();
	virtual ~SortingTests();

	void run_test();

private:
	template<class Pheet, template <class P> class Sorter>
	void run_sorter();
};

template <class Pheet, template <class P> class Sorter>
void SortingTests::run_sorter() {
//	std::cout << "test\tsorter\tscheduler\ttype\tsize\tseed\tcpus\ttotal_time\truns\t";
//	Sorter::print_headers();
//	std::cout << std::endl;
#ifdef SORTING_TEST
//	std::cout << (std::numeric_limits<procs_t>::max() >> 1) << std::endl;
//	std::cout << Sorter::max_cpus << std::endl;

	for(size_t t = 0; t < sizeof(sorting_test_types)/sizeof(sorting_test_types[0]); t++) {
		for(size_t n = 0; n < sizeof(sorting_test_n)/sizeof(sorting_test_n[0]); n++) {
			for(size_t c = 0; c < sizeof(sorting_test_cpus)/sizeof(sorting_test_cpus[0]); c++) {
				if(sorting_test_cpus[c] <= Pheet::Environment::max_cpus) {
					for(size_t s = 0; s < sizeof(sorting_test_seeds)/sizeof(sorting_test_seeds[0]); s++) {
						SortingTest<Pheet, Sorter> st(sorting_test_cpus[c], sorting_test_types[t], sorting_test_n[n], sorting_test_seeds[s]);
						st.run_test();
					}
				}
			}
		}
	}
#endif
}

}

#endif /* SORTINGTESTS_H_ */
