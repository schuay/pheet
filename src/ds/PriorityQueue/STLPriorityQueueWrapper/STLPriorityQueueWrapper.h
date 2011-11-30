/*
 * STLPriorityQueueWrapper.h
 *
 *  Created on: Nov 30, 2011
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef STLPRIORITYQUEUEWRAPPER_H_
#define STLPRIORITYQUEUEWRAPPER_H_

#include "../../../settings.h"
#include <vector>
#include <queue>
#include <iostream>

namespace pheet {

template <typename TT, class Comparator = std::less<TT> >
class STLPriorityQueueWrapper {
public:
	typedef TT T;

	STLPriorityQueueWrapper();
	STLPriorityQueueWrapper(Comparator const& comp);
	~STLPriorityQueueWrapper();

	void push(T item);
	T peek();
	T pop();

	size_t get_length();
	bool is_empty();

	static void print_name();

private:
	std::priority_queue<T, std::vector<T>, Comparator> stl_pq;
};


template <typename TT, class Comparator>
STLPriorityQueueWrapper<TT, Comparator>::STLPriorityQueueWrapper() {

}

template <typename TT, class Comparator>
STLPriorityQueueWrapper<TT, Comparator>::STLPriorityQueueWrapper(Comparator const& comp)
: stl_pq(comp) {

}

template <typename TT, class Comparator>
STLPriorityQueueWrapper<TT, Comparator>::~STLPriorityQueueWrapper() {

}

template <typename TT, class Comparator>
void STLPriorityQueueWrapper<TT, Comparator>::push(T item) {
	stl_pq.push(item);
}

template <typename TT, class Comparator>
TT STLPriorityQueueWrapper<TT, Comparator>::peek() {
	return stl_pq.top();
}


template <typename TT, class Comparator>
TT STLPriorityQueueWrapper<TT, Comparator>::pop() {
	T ret = stl_pq.top();
	stl_pq.pop();
	return ret;
}

template <typename TT, class Comparator>
size_t STLPriorityQueueWrapper<TT, Comparator>::get_length() {
	return stl_pq.size();
}

template <typename TT, class Comparator>
bool STLPriorityQueueWrapper<TT, Comparator>::is_empty() {
	return stl_pq.empty();
}

template <typename TT, class Comparator>
void STLPriorityQueueWrapper<TT, Comparator>::print_name() {
	std::cout << "STLPriorityQueueWrapper";
}

}

#endif /* STLPRIORITYQUEUEWRAPPER_H_ */
