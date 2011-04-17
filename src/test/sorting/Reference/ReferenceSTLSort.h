/*
 * ReferenceSTLSort.h
 *
 *  Created on: 07.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef REFERENCESTLSORT_H_
#define REFERENCESTLSORT_H_

#include <algorithm>
#include "../../../misc/types.h"

namespace pheet {

class ReferenceSTLSort {
public:
	ReferenceSTLSort(unsigned int* data, size_t length);
	~ReferenceSTLSort();

	void sort();

	static const procs_t max_cpus;
	static const char name[];
	static const char scheduler_name[];

private:
	unsigned int* start;
	unsigned int* end;
};

}

#endif /* REFERENCESTLSORT_H_ */
