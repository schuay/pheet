/*
 * ReferenceQuicksort.cpp
 *
 *  Created on: 21.04.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "../../../settings.h"
#include "ReferenceQuicksort.h"

#include <functional>
#include <algorithm>
#include <assert.h>
#include <iostream>

namespace pheet {

const procs_t ReferenceQuicksort::max_cpus = 1;
const char ReferenceQuicksort::name[] = "ReferenceQuicksort";
const char ReferenceQuicksort::scheduler_name[] = "none";

ReferenceQuicksort::ReferenceQuicksort(procs_t cpus, unsigned int* data, size_t length)
: data(data), length(length) {
	assert(cpus == 1);
}

ReferenceQuicksort::~ReferenceQuicksort() {

}

void ReferenceQuicksort::sort() {
	sort(data, length);
}

void ReferenceQuicksort::sort(unsigned int* data, size_t length) {
	if(length <= 1)
		return;

	unsigned int * middle = std::partition(data, data + length - 1,
		std::bind2nd(std::less<unsigned int>(), *(data + length - 1)));
	size_t pivot = middle - data;
	std::swap(*(data + length - 1), *middle);    // move pivot to middle

	sort(data, pivot);
	sort(data + pivot + 1, length - pivot - 1);
}

void ReferenceQuicksort::print_results() {

}

void ReferenceQuicksort::print_headers() {

}

void ReferenceQuicksort::print_scheduler_name() {
	std::cout << scheduler_name;
}

}
