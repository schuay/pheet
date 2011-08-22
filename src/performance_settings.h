/*
 * performance_settings.h
 *
 *  Created on: 11.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef PERFORMANCE_SETTINGS_H_
#define PERFORMANCE_SETTINGS_H_

namespace pheet {

#ifndef NDEBUG

bool const scheduler_count_spawns = true;
bool const scheduler_count_spawns_to_call = true;
bool const scheduler_count_calls = true;
bool const scheduler_count_finishes = true;
bool const stealing_deque_count_steals = true;
bool const stealing_deque_count_steal_calls = true;
bool const stealing_deque_count_unsuccessful_steal_calls = true;
bool const stealing_deque_count_pop_cas = true;
bool const scheduler_measure_total_time = true;
bool const scheduler_measure_task_time = true;
bool const scheduler_measure_idle_time = true;

#else

bool const scheduler_count_spawns = false;
bool const scheduler_count_spawns_to_call = false;
bool const scheduler_count_calls = false;
bool const scheduler_count_finishes = false;
bool const stealing_deque_count_steals = false;
bool const stealing_deque_count_steal_calls = false;
bool const stealing_deque_count_unsuccessful_steal_calls = false;
bool const stealing_deque_count_pop_cas = false;
bool const scheduler_measure_total_time = false;
bool const scheduler_measure_task_time = false;
bool const scheduler_measure_idle_time = false;

#endif

}

#endif /* PERFORMANCE_SETTINGS_H_ */
