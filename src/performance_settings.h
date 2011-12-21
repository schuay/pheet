/*
 * performance_settings.h
 *
 *  Created on: 11.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef PHEET_PERFORMANCE_SETTINGS_H_
#define PHEET_PERFORMANCE_SETTINGS_H_

namespace pheet {

#ifndef NDEBUG

bool const scheduler_count_tasks_at_level = true;
bool const scheduler_count_steal_calls_per_thread = true;
bool const scheduler_count_unsuccessful_steal_calls_per_thread = true;
bool const scheduler_count_spawns = true;
bool const scheduler_count_actual_spawns = true;
bool const scheduler_count_spawns_to_call = true;
bool const scheduler_count_calls = true;
bool const scheduler_count_finishes = true;
bool const scheduler_count_completion_signals = true;
bool const scheduler_count_chained_completion_signals = true;
bool const scheduler_count_remote_chained_completion_signals = true;
bool const scheduler_count_non_blocking_finish_regions = true;
bool const stealing_deque_count_steals = true;
bool const stealing_deque_count_steal_calls = true;
bool const stealing_deque_count_unsuccessful_steal_calls = true;
bool const stealing_deque_count_pop_cas = true;

bool const stealing_deque_count_dequeued_tasks = true;
bool const stealing_deque_count_steal_executed_tasks = true;

bool const scheduler_measure_total_time = true;
bool const scheduler_measure_task_time = true;
bool const scheduler_measure_sync_time = true;
bool const scheduler_measure_idle_time = true;
bool const scheduler_measure_queue_processing_time = true;
bool const scheduler_measure_visit_partners_time = true;
bool const scheduler_measure_wait_for_finish_time = true;
bool const scheduler_measure_wait_for_coordinator_time = true;

bool const task_storage_count_steals = true;
bool const task_storage_count_unsuccessful_pops = true;
bool const task_storage_count_successful_pops = true;
bool const task_storage_count_unsuccessful_takes = true;
bool const task_storage_count_successful_takes = true;
bool const task_storage_count_unsuccessful_steals = true;
bool const task_storage_count_successful_steals = true;
bool const task_storage_count_size_pop = true;
bool const task_storage_count_size_steal = true;
bool const task_storage_measure_push_time = true;
bool const task_storage_measure_pop_time = true;
bool const task_storage_measure_clean_time = true;
bool const task_storage_measure_create_control_block_time = true;
bool const task_storage_measure_configure_successor_time = true;
bool const task_storage_measure_heap_push_time = true;
bool const task_storage_measure_put_time = true;
bool const task_storage_measure_strategy_alloc_time = true;
bool const task_storage_measure_steal_time = true;
bool const task_storage_count_skipped_cleanups = true;
bool const task_storage_measure_max_control_block_items = true;
bool const task_storage_measure_max_heap_length = true;

bool const scheduler_measure_finish_stack_nonblocking_max = true;
bool const scheduler_measure_finish_stack_blocking_min = true;

#else

bool const scheduler_count_tasks_at_level = false;
bool const scheduler_count_steal_calls_per_thread = false;
bool const scheduler_count_unsuccessful_steal_calls_per_thread = false;
bool const scheduler_count_spawns = false;
bool const scheduler_count_actual_spawns = false;
bool const scheduler_count_spawns_to_call = false;
bool const scheduler_count_calls = false;
bool const scheduler_count_finishes = false;
bool const scheduler_count_completion_signals = false;
bool const scheduler_count_chained_completion_signals = false;
bool const scheduler_count_remote_chained_completion_signals = false;
bool const scheduler_count_non_blocking_finish_regions = false;
bool const stealing_deque_count_steals = false;
bool const stealing_deque_count_steal_calls = false;
bool const stealing_deque_count_unsuccessful_steal_calls = false;
bool const stealing_deque_count_pop_cas = false;

bool const stealing_deque_count_dequeued_tasks = false;
bool const stealing_deque_count_steal_executed_tasks = false;

bool const scheduler_measure_total_time = false;
bool const scheduler_measure_task_time = false;
bool const scheduler_measure_sync_time = false;
bool const scheduler_measure_idle_time = false;
bool const scheduler_measure_queue_processing_time = false;
bool const scheduler_measure_visit_partners_time = false;
bool const scheduler_measure_wait_for_finish_time = false;
bool const scheduler_measure_wait_for_coordinator_time = false;

bool const task_storage_count_steals = false;
bool const task_storage_count_unsuccessful_pops = false;
bool const task_storage_count_successful_pops = false;
bool const task_storage_count_unsuccessful_takes = false;
bool const task_storage_count_successful_takes = false;
bool const task_storage_count_unsuccessful_steals = false;
bool const task_storage_count_successful_steals = false;
bool const task_storage_count_size_pop = false;
bool const task_storage_count_size_steal = false;
bool const task_storage_measure_push_time = false;
bool const task_storage_measure_pop_time = false;
bool const task_storage_measure_clean_time = false;
bool const task_storage_measure_create_control_block_time = false;
bool const task_storage_measure_configure_successor_time = false;
bool const task_storage_measure_heap_push_time = false;
bool const task_storage_measure_put_time = false;
bool const task_storage_measure_strategy_alloc_time = false;
bool const task_storage_measure_steal_time = false;
bool const task_storage_count_skipped_cleanups = false;
bool const task_storage_measure_max_control_block_items = false;
bool const task_storage_measure_max_heap_length = false;

bool const scheduler_measure_finish_stack_nonblocking_max = false;
bool const scheduler_measure_finish_stack_blocking_min = false;

#endif

}

#endif /* PHEET_PERFORMANCE_SETTINGS_H_ */
