/*
 * LUPivTests.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVTESTS_H_
#define LUPIVTESTS_H_

#include "../Test.h"
#include "../../test_settings.h"
#include "LUPivTest.h"

namespace pheet {

template <bool ACTIVE> class LUPivTests;

template <>
class LUPivTests<false> : Test {
public:
	LUPivTests() {}
	virtual ~LUPivTests() {}

	void run_test() {}

private:
	template<class Kernel>
	void run_sorter() {}
};

template <>
class LUPivTests<true> : Test {
public:
	LUPivTests();
	virtual ~LUPivTests();

	void run_test();

private:
	template<class Kernel>
	void run_kernel();
};

template <class Kernel>
void LUPivTests<true>::run_kernel() {
	for(size_t t = 0; t < sizeof(lupiv_test_types)/sizeof(lupiv_test_types[0]); t++) {
		for(size_t n = 0; n < sizeof(lupiv_test_n)/sizeof(lupiv_test_n[0]); n++) {
			for(size_t c = 0; c < sizeof(lupiv_test_cpus)/sizeof(lupiv_test_cpus[0]); c++) {
				if(lupiv_test_cpus[c] <= Kernel::max_cpus) {
					for(size_t s = 0; s < sizeof(lupiv_test_seeds)/sizeof(lupiv_test_seeds[0]); s++) {
						LUPivTest<Kernel> st(lupiv_test_cpus[c], lupiv_test_types[t], lupiv_test_n[n], lupiv_test_seeds[s]);
						st.run_test();
					}
				}
			}
		}
	}
}


}

#endif /* LUPIVTESTS_H_ */
