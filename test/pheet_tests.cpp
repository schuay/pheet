/*
 * pheet_tests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#define NDEBUG 1

#include "init.h"
#include "sorting/SortingTests.h"
#include "graph_bipartitioning/GraphBipartitioningTests.h"
#include "lupiv/LUPivTests.h"
#include "inarow/InARowTests.h"
#include "n-queens/NQueensTests.h"
#include "lupiv/LUPivTests.h"
#include "uts/UTSTests.h"
#include "sor/SORTests.h"
#include "tristrip/TriStripTests.h"
#include "prefix_sum/PrefixSumTests.h"
#include "sssp/SsspTests.h"
#include "set_bench/SetBench.h"
#include "count_bench/CountBench.h"
#include <map>
#include <string>

using namespace pheet;

int main(int argc, char* argv[]) {

	std::map<std::string, Tests*> tests;

/*	LUPivTests lpt;
	SORTests sors;
	TriStripTests tristrip;

	tests["SOR"] = &sors;
	tests["lupiv"] = &lpt;
	tests["tristrip"] = &tristrip;
	
	if(argc==3)
	  {
	    bool prio;
	    if(std::string(argv[2])=="Strategy")
	      prio = true;
	    else
	      if(std::string(argv[2])=="Basic")
		prio = false;
	      else
		{
		  std::cout << "No such scheduler" << std::endl;
		  return 1;
		}
	    
	    if(tests.find(argv[1])==tests.end())
	      {
		return 1;
		std::cout << "No such test" << std::endl;
	      }
	    tests[argv[1]]->run_test(prio);
	    return 0;
	  }*/
	
	SortingTests st;
	st.run_test();

  	GraphBipartitioningTests gpt;
  	gpt.run_test();
   
  	PrefixSumTests pt;
  	pt.run_test();

	InARowTests iarts;
	iarts.run_test();

	NQueensTests nqt;
	nqt.run_test();

	UTSTests utss;
	utss.run_test();
	
	PrefixSumTests pst;
	pst.run_test();

	SsspTests sssp;
	sssp.run_test();

	SetBench sb;
	sb.run_test();

	CountBench cb;
	cb.run_test();

	return 0;
}
