/*
 * settings.h
 *
 *  Created on: 28.03.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PHEET_SETTINGS_H_
#define PHEET_SETTINGS_H_

#include "misc/assert.h"
#include "environment.h"
#include "misc/types.h"

#ifdef __GNUG__
#define thread_local __thread
#elif __clang__
#define thread_local __thread
#endif

#include "performance_settings.h"

#endif /* PHEET_SETTINGS_H_ */
