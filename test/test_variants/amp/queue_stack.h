/*
 * queue_stack.h
 *
 *  Created on: May 29, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef QUEUE_STACK_H_
#define QUEUE_STACK_H_

namespace pheet {

#define AMP_QUEUE_STACK_TEST true

#define SORTING_TEST true
const procs_t sorting_test_cpus[] = {1, 2, 3, 6, 12, 24, 48, 96};
const unsigned int sorting_test_seeds[] = {0, 1};
const size_t sorting_test_n[] = {1000000, 10000000};
const int sorting_test_types[] = {0, 1};

#define GRAPH_BIPARTITIONING_TEST true
const procs_t graph_bipartitioning_test_cpus[] = {1, 2, 3, 6, 12, 24, 48, 96};
const unsigned int graph_bipartitioning_test_seeds[] = {0, 1};
const GraphBipartitioningProblem graph_bipartitioning_test_problems[] = {
		// n, p, max_w
		{35, 0.5, 1},
		{35, 0.5, 1000},
		{38, 0.5, 1000}
};
const int graph_bipartitioning_test_types[] = {0};
}

#endif /* QUEUE_STACK_H_ */
