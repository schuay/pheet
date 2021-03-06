/*
 * sequential.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#include <functional>
#include <algorithm>
#include <random>
#include <limits>
#include <iostream>

size_t const N = 40000000;

void quicksort(int* begin, int* end) {
	if(end - begin <= 1)
		return;

	int* middle = std::partition(begin, end - 1,
			std::bind2nd(std::less<int>(), *(end - 1)));
	std::swap(*(end - 1), *middle);    // move pivot to middle

	quicksort(begin, middle);
	quicksort(middle + 1, end);
}

bool verify(int* begin, int* end) {
	int last = *begin;
	for(auto i = begin; i != end; ++i) {
		if(*i < last)
			return false;
		last = *i;
	}
	return true;
}

int main(int, char**) {
	int* data = new int[N];
	std::mt19937 rng;
  	rng.seed(0);

  	std::uniform_int_distribution<int> rnd_st(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

  	for(size_t i = 0; i < N; ++i)
  		data[i] = rnd_st(rng);

  	// start quicksort
  	quicksort(data, data + N);

  	if(verify(data, data + N)) {
  		std::cout << "Sorted" << std::endl;
  	}
  	else {
  		std::cout << "Sorting not successful" << std::endl;
  	}
}



