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

#else

bool const scheduler_count_spawns = false;
bool const scheduler_count_spawns_to_call = false;
bool const scheduler_count_calls = false;
bool const scheduler_count_finishes = false;

#endif

}

#endif /* PERFORMANCE_SETTINGS_H_ */
