/*
 * ReferenceSTLSort.h
 *
 *  Created on: 07.04.2011
 *      Author: mwimmer
 */

#ifndef REFERENCESTLSORT_H_
#define REFERENCESTLSORT_H_

#include <algorithm>
#include "../../../misc/types.h"

class ReferenceSTLSort {
public:
	ReferenceSTLSort(unsigned int* data, size_t length);
	virtual ~ReferenceSTLSort();

	void sort();

	static const procs_t max_cpus;
	static const char name[];

private:
	unsigned int* start;
	unsigned int* end;
};


#endif /* REFERENCESTLSORT_H_ */
