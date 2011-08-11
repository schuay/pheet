/*
 * settings.h
 *
 *  Created on: 28.03.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "src/misc/types.h"

//#define NDEBUG

const pheet::procs_t system_max_cpus = 2;


#define ENV_LINUX_GCC	1
//#define ENV_LINUX_ICC	1
//#define_ENV_SOLARIS_SUNCC	1

#define ENV_X86		1
//#define ENV_SPARC	1

#include "performance_settings.h"

#endif /* SETTINGS_H_ */
