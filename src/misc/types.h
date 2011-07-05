/*
 * types.h
 *
 *  Created on: 06.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stddef.h>

namespace pheet {

typedef size_t procs_t;
#define PROCST_FETCH_AND_SUB(p, val)		(SIZET_FETCH_AND_SUB(p, val))

}

#endif /* TYPES_H_ */
