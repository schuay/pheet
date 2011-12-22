/*
 * pheet_tests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "sorting/SortingTests.h"
#include "graph_bipartitioning/GraphBipartitioningTests.h"
#include "inarow/InARowTests.h"
#include "n-queens/NQueensTests.h"
#include "lupiv/LUPivTests.h"
#include "uts/UTSTests.h"
#include "sor/SORTests.h"

using namespace pheet;

int main(int argc, char* argv[]) {
  	SortingTests<sorting_test> st;
  	st.run_test();

  	GraphBipartitioningTests gpt;
  	gpt.run_test();

	InARowTests iarts;
	iarts.run_test();

	NQueensTests nqt;
	nqt.run_test();

	LUPivTests<lupiv_test> lpt;
	lpt.run_test();

	UTSTests utss;
	utss.run_test();

	SORTests sors;
	sors.run_test();

	return 0;
}
