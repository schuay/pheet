/*
 * LUPivTests.h
 *
 *  Created on: Dec 20, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef LUPIVTESTS_H_
#define LUPIVTESTS_H_

#include "../init.h"
#include "../Test.h"
#include "../init.h"
#ifdef LUPIV_TEST
#include "LUPivTest.h"
#endif

namespace pheet {

class LUPivTests : Test {
public:
	LUPivTests();
	virtual ~LUPivTests();

	void run_test();

private:
	template<class Pheet, template <class P> class Kernel>
	void run_kernel();
};

template <class Pheet, template <class P> class Kernel>
void LUPivTests::run_kernel() {
#ifdef LUPIV_TEST
	for(size_t t = 0; t < sizeof(lupiv_test_types)/sizeof(lupiv_test_types[0]); t++) {
		for(size_t n = 0; n < sizeof(lupiv_test_n)/sizeof(lupiv_test_n[0]); n++) {
			for(size_t c = 0; c < sizeof(lupiv_test_cpus)/sizeof(lupiv_test_cpus[0]); c++) {
				if(lupiv_test_cpus[c] <= Pheet::Environment::max_cpus) {
					for(size_t s = 0; s < sizeof(lupiv_test_seeds)/sizeof(lupiv_test_seeds[0]); s++) {
						LUPivTest<Pheet, Kernel> st(lupiv_test_cpus[c], lupiv_test_types[t], lupiv_test_n[n], lupiv_test_seeds[s]);
						st.run_test();
					}
				}
			}
		}
	}
#endif
}


}

#endif /* LUPIVTESTS_H_ */