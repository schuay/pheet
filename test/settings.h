/*
 * settings.h
 *
 *  Created on: Feb 15, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef PHEET_TEST_SETTINGS_H_
#define PHEET_TEST_SETTINGS_H_

//#define PHEET_DEBUG_MODE
// #define PHEET_ALL_PERFORMANCE_COUNTERS
#define ACTIVE_TEST 	"test_variants/daniel.h"


bool const graph_bipartitioning_test_count_irrelevant_tasks = true;
bool const graph_bipartitioning_test_count_upper_bound_changes = true;
bool const graph_bipartitioning_test_count_allocated_subproblems = false;
bool const graph_bipartitioning_test_measure_memory_allocation_time = false;

bool const sor_slices_rescheduled_at_same_place = true;

#endif /* PHEET_TEST_SETTINGS_H_ */
