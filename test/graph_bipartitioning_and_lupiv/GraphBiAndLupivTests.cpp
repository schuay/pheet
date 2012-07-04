/*
* GraphBiAndLupivTests.cpp
*
*  Created on: 5.12.2011
*      Author: Daniel Cederman
*     License: Boost Software License 1.0 (BSL1.0)
*/

#include "GraphBiAndLupivTests.h"

#include "../init.h"
#include "../Test.h"


#include "../lupiv/LUPivTest.h"
#include "../lupiv/Simple/SimpleLUPiv.h"
#include "../lupiv/LocalityStrategy/LocalityStrategyLUPiv.h"


#include "../graph_bipartitioning/GraphBipartitioningTest.h"
#include "../graph_bipartitioning/ImprovedBranchBound/ImprovedBranchBoundGraphBipartitioning.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioning.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningBestFirstStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDepthFirstBestStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningLowerBoundStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperBoundFifoStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningUpperLowerBoundStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningAutoStrategy.h"
#include "../graph_bipartitioning/ImprovedStrategyBranchBound/ImprovedStrategyBranchBoundGraphBipartitioningDynamicStrategy.h"


#include <pheet/sched/Strategy/StrategyScheduler.h>
#include <pheet/sched/Basic/BasicScheduler.h>

namespace pheet {
  
  template <class Pheet, template <class P1> class Kernel1, template <class P2> class Kernel2>
  void GraphBiAndLupivTests::test()
  {
    typename Pheet::MachineModel mm;
    procs_t max_cpus = std::min(mm.get_num_leaves(), Pheet::Environment::max_cpus);
    
    for(size_t t = 0; t < sizeof(lupiv_test_types)/sizeof(lupiv_test_types[0]); t++) {
    	for(size_t n = 0; n < sizeof(lupiv_test_n)/sizeof(lupiv_test_n[0]); n++) {
    		for(size_t s = 0; s < sizeof(lupiv_test_seeds)/sizeof(lupiv_test_seeds[0]); s++) {
    			for(size_t r = 0; r < sizeof(graph_bipartitioning_test_types)/sizeof(graph_bipartitioning_test_types[0]); r++) {
    				for(size_t pr = 0; pr < sizeof(graph_bipartitioning_test_problems)/sizeof(graph_bipartitioning_test_problems[0]); pr++) {
    					for(size_t gs = 0; gs < sizeof(graph_bipartitioning_test_seeds)/sizeof(graph_bipartitioning_test_seeds[0]); gs++) {
		
								bool max_processed = false;
								procs_t cpus;
								for(size_t c = 0; c < sizeof(lupiv_test_cpus)/sizeof(lupiv_test_cpus[0]); c++) {
									cpus = lupiv_test_cpus[c];
									if(cpus >= max_cpus) {
										if(!max_processed) {
											cpus = max_cpus;
											max_processed = true;
										}
										else {
											continue;
										}
									}
		  
									GraphBipartitioningTest<Pheet, Kernel1> gbt(cpus, graph_bipartitioning_test_types[r],
											graph_bipartitioning_test_problems[pr].n,
											graph_bipartitioning_test_problems[pr].p,
											graph_bipartitioning_test_problems[pr].max_w,
											graph_bipartitioning_test_seeds[gs]);
		  
								//	LUPivTest<Pheet, Kernel2> st(lupiv_test_cpus[c], lupiv_test_types[t], lupiv_test_n[n], lupiv_test_seeds[s]);
									gbt.run_test();
								//	st.run_test();
								}
    					}
    				}
    			}
    		}
    	}
    }
  }
  
  void GraphBiAndLupivTests::run_test(bool usestrategy)
  {
    if(graphbiandlupiv_test) {
      if(usestrategy)
	//	test<Pheet::WithScheduler<StrategyScheduler>, ImprovedBranchBoundGraphBipartitioning,LocalityStrategyLUPiv >();
	//      else
	//	test<Pheet::WithScheduler<BasicScheduler>,ImprovedBranchBoundGraphBipartitioning,LocalityStrategyLUPiv >();
	test<Pheet,ImprovedBranchBoundGraphBipartitioning,LocalityStrategyLUPiv >();

    }
  }
  
}

