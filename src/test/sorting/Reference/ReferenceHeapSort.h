/*
 * ReferenceHeapSort.h
 *
 *  Created on: Nov 30, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef REFERENCEHEAPSORT_H_
#define REFERENCEHEAPSORT_H_

#include "../../../settings.h"

namespace pheet {

template <template <typename T, typename Comp> class PriorityQueue>
class ReferenceHeapSort {
public:
	ReferenceHeapSort(procs_t cpus, unsigned int* data, size_t length);
	~ReferenceHeapSort();

	void sort();
	void print_results();

	static void print_name();

	static void print_headers();

	static void print_scheduler_name();

	static const procs_t max_cpus;
	static const char name[];
	static const char scheduler_name[];

private:
	void sort(unsigned int* data, size_t length);

	unsigned int* data;
	size_t length;
};

template <template <typename T, typename Comp> class PriorityQueue>
const procs_t ReferenceHeapSort<PriorityQueue>::max_cpus = 1;

template <template <typename T, typename Comp> class PriorityQueue>
const char ReferenceHeapSort<PriorityQueue>::name[] = "ReferenceHeapSort";

template <template <typename T, typename Comp> class PriorityQueue>
const char ReferenceHeapSort<PriorityQueue>::scheduler_name[] = "none";

template <template <typename T, typename Comp> class PriorityQueue>
ReferenceHeapSort<PriorityQueue>::ReferenceHeapSort(procs_t cpus, unsigned int* data, size_t length)
: data(data), length(length) {
	assert(cpus == 1);
}

template <template <typename T, typename Comp> class PriorityQueue>
ReferenceHeapSort<PriorityQueue>::~ReferenceHeapSort() {

}

template <template <typename T, typename Comp> class PriorityQueue>
void ReferenceHeapSort<PriorityQueue>::sort() {
	sort(data, length);
}

template <template <typename T, typename Comp> class PriorityQueue>
void ReferenceHeapSort<PriorityQueue>::sort(unsigned int* data, size_t length) {
	if(length <= 1)
		return;

	PriorityQueue<unsigned int, std::less<unsigned int> > pq;

	for(size_t i = 0; i < length; ++i) {
		pq.push(data[i]);
	}

	while(!pq.is_empty()) {
		unsigned int tmp = pq.pop();
		data[pq.get_length()] = tmp;
	}
}

template <template <typename T, typename Comp> class PriorityQueue>
void ReferenceHeapSort<PriorityQueue>::print_results() {

}

template <template <typename T, typename Comp> class PriorityQueue>
void ReferenceHeapSort<PriorityQueue>::print_headers() {

}

template <template <typename T, typename Comp> class PriorityQueue>
void ReferenceHeapSort<PriorityQueue>::print_scheduler_name() {
	std::cout << scheduler_name;
}

}

#endif /* REFERENCEHEAPSORT_H_ */
