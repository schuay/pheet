/*
 * ReferenceSTLSort
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "../../../../settings.h"
#include "ReferenceSTLSort.h"

#include <assert.h>

namespace pheet {

const procs_t ReferenceSTLSort::max_cpus = 1;
const char ReferenceSTLSort::name[] = "ReferenceSTLSort";
const char ReferenceSTLSort::scheduler_name[] = "none";


ReferenceSTLSort::ReferenceSTLSort(procs_t cpus, unsigned int* data, size_t length)
: start(data), end(data + length) {
	assert(cpus == 1);
}

ReferenceSTLSort::~ReferenceSTLSort() {

}

void ReferenceSTLSort::sort() {
	std::sort(start, end);
}

}
