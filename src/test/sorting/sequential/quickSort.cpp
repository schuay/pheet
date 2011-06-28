/*
 * quickSort.cpp
 *
 *  Created on: 04.05.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include <functional>
#include <algorithm>

#include "quickSort.h"

namespace peppher {

void quickSort(unsigned int* data, size_t length) {
	if(length <= 1)
		return;

	unsigned int * middle = std::partition(data, data + length - 1,
		std::bind2nd(std::less<unsigned int>(), *(data + length - 1)));
	size_t pivot = middle - data;
	std::swap(*(data + length - 1), *middle);    // move pivot to middle

	quickSort(data, pivot);
	quickSort(data + pivot + 1, length - pivot - 1);
}

}
