/*
 * pheet_tests.cpp
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "sorting/SortingTests.h"
#include "graph_bipartitioning/GraphBipartitioningTests.h"
#include "n-queens/NQueensTests.h"

using namespace pheet;

int main(int argc, char* argv[]) {

	SortingTests st;
	st.run_test();

	GraphBipartitioningTests gpt;
	gpt.run_test();

	NQueensTests nqt;
	nqt.run_test();

	return 0;
}
