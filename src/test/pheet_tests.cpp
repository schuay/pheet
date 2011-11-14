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

using namespace pheet;

int main(int argc, char* argv[]) {

  InARowTests iarts;
  iarts.run_test();

  return 0;
	SortingTests st;
	st.run_test();

	GraphBipartitioningTests gpt;
	gpt.run_test();

	return 0;
}
