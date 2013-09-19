/*
 * TriStripTests.cpp
 *
 *  Created on: 5.12.2011
 *      Author: Daniel Cederman
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#include "TriStripTests.h"

#ifdef TriStrip_TEST

#include "TriStripTest.h"
#include "SGITriStrip/TriStripRun.h"

#include <pheet/sched/Strategy/StrategyScheduler.h>
#include <pheet/sched/Basic/BasicScheduler.h>

//#include "../test_schedulers.h"
#include <iostream>

#endif

namespace pheet {

template<class Test>
void TriStripTests::test(bool) {
#ifdef TriStrip_TEST
	for (size_t nc = 0; nc< sizeof(tristrip_test_nodecount)/ sizeof(tristrip_test_nodecount[0]); nc++) {
//		for(size_t sl = 0; sl < sizeof(sor_test_slices)/sizeof(sor_test_slices[0]); sl++) {
//			for(size_t o = 0; o < sizeof(sor_test_omega)/sizeof(sor_test_omega[0]); o++) {
//				for(size_t ms = 0; ms < sizeof(sor_test_maxtrissize)/sizeof(sor_test_maxtrissize[0]); ms++) {
		for (size_t c = 0;c < sizeof(tristrip_test_cpus) / sizeof(tristrip_test_cpus[0]);c++) {
		  TriStripTest<Test> iart(tristrip_test_cpus[c], tristrip_test_nodecount[nc], prio);
			printf("Run\n");
			iart.run_test();
		}
	}
//			}
//		}
//	  }
#endif
}

void TriStripTests::run_test(bool) {
#ifdef TriStrip_TEST
	if (tristrip_test) {

		//			  for(size_t pp = 0; pp < sizeof(sor_prio)/sizeof(sor_prio[0]); pp++) {
		if (usestrategy)
			test<TriStripRun<Pheet::WithScheduler<StrategyScheduler> > >(true);
				else
			test<TriStripRun<Pheet::WithScheduler<BasicScheduler> > >(true);
		//			  }
	}
#endif
}

}
