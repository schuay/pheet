/*
 * FibonacciHeap.h
 *
 *  Created on: Jun 11, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef FIBONACCIHEAP_H_
#define FIBONACCIHEAP_H_

namespace pheet {

template <typename TT>
struct FibonacciHeapNode {
	typedef FibonacciHeapNode<TT> Self;

	TT data;
	Self* children;
	Self* prev;
	Self* next;
	uint8_t d;
};

template <class Pheet, typename TT, class Comparator = std::less<TT> >
class FibonacciHeap {
public:
	typedef TT T;
	typedef FibonacciHeapNode<TT> Node;

	FibonacciHeap()
	:max(nullptr) {}

	~FibonacciHeap() {
		if(max != nullptr) {
			delete_node_list(max);
		}
	}

	void push(T item) {
		Node* tmp = new Node;
		tmp->d = 0;
		tmp->data = item;
		tmp->children = nullptr;
		if(max == nullptr) {
			tmp->next = tmp;
			tmp->prev = tmp;
			max = tmp;
		}
		else {
			tmp->prev = max->prev;
			max->prev->next = tmp;
			max->prev = tmp;
			tmp->next = max;

			if(is_less(max->data, item)) {
				max = tmp;
			}
		}
	}

	TT peek() {
		return max->data;
	}

	TT pop() {
		T ret = max->data;

		combine(max, max->children);

		Node* next = max->next;
		next->prev = max->prev;
		max->prev->next = next;

		delete max;
		max = next;

		consolidate();

		return ret;
	}

	bool empty() {
		return max == nullptr;
	}
	bool is_empty() {
		return empty();
	}
	size_t size() {
		return _size;
	}
	size_t get_length() {
		return _size;
	}


private:
	void delete_node_list(Node* node) {
		Node* end = node;

		do {
			if(node->children != nullptr) {
				delete_node_list(node->children);
			}
			Node *tmp = node->next;
			delete node;
			node = tmp;
		} while(node != end);
	}

	void combine(Node* list1, Node* list2) {
//		Node* tmp = list1->next;
		list1->prev->next = list2;
		list2->prev->next = list1;
		std::swap(list1->prev, list2->prev);
	}

	void consolidate() {
		pheet_assert(max != nullptr);

		size_t init = 0;
		Node* helper[64];

		Node* node = max->prev;
		Node* end = node;
		do {
			node = node->next;
			for(; init <= node->d; ++init) {
				helper[init] = nullptr;
			}
			if(is_less(max->data, node->data)) {
				max = node;
			}
			if(helper[node->d] == nullptr) {
				helper[node->d] = node;
			}
			else if(is_less(helper[node->d]->data, node->data)){
				// TODO
			}
			else {
				// TODO
			}
		} while(node != end);
	}

	Node* max;
	size_t _size;
	Comparator is_less;
};

} /* namespace pheet */
#endif /* FIBONACCIHEAP_H_ */
