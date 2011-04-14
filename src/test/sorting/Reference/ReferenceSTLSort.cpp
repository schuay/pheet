/*
 * ReferenceSTLSort
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#include "ReferenceSTLSort.h"

namespace pheet {

const procs_t ReferenceSTLSort::max_cpus = 1;
const char ReferenceSTLSort::name[] = "ReferenceSTLSort";


ReferenceSTLSort::ReferenceSTLSort(unsigned int* data, size_t length)
: start(data), end(data + length) {

}

ReferenceSTLSort::~ReferenceSTLSort() {

}

void ReferenceSTLSort::sort() {
	std::sort(start, end);
}

}
