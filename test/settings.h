/*
 * settings.h
 *
 *  Created on: Feb 15, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PHEET_TEST_SETTINGS_H_
#define PHEET_TEST_SETTINGS_H_

// #define PHEET_DEBUG_MODE
// #define PHEET_ALL_PERFORMANCE_COUNTERS
#define ACTIVE_TEST 	"test_variants/full.h"


bool const graph_bipartitioning_test_count_irrelevant_tasks = false;
bool const graph_bipartitioning_test_count_upper_bound_changes = false;
bool const graph_bipartitioning_test_count_allocated_subproblems = false;
bool const graph_bipartitioning_test_measure_memory_allocation_time = false;


#endif /* PHEET_TEST_SETTINGS_H_ */