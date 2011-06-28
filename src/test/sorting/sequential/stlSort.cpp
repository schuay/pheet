/*
 * stlSort.cpp
 *
 *  Created on: 04.05.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include <algorithm>

#include "stlSort.h"

namespace peppher {

void stlSort(unsigned int* data, size_t length) {
	std::sort(data, data + length);
}

}
