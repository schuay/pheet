/*
 * settings.h
 *
 *  Created on: 28.03.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef PHEET_SETTINGS_H_
#define PHEET_SETTINGS_H_

#ifndef NDEBUG
#ifndef PHEET_DEBUG_MODE
#define NDEBUG
#define PHEET_UNDEF_NDEBUG
#endif
#else
#ifndef PHEET_DEBUG_MODE
#undef NDEBUG
#define PHEET_DEF_NDEBUG
#endif
#endif

#include "environment.h"
#include "misc/types.h"

#ifdef __GNUG__
#define thread_local __thread
#elif __clang__
#define thread_local __thread
#endif

#include "performance_settings.h"

#endif /* PHEET_SETTINGS_H_ */
