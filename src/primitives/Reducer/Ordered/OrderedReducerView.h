/*
 * OrderedReducerView.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef ORDEREDREDUCERVIEW_H_
#define ORDEREDREDUCERVIEW_H_

/*
 *
 */
namespace pheet {

template <typename T, template <typename S> class Operation>
class OrderedReducerView {
public:
	OrderedReducerView(OrderedReducerView<T, Operation>* pred, OrderedReducerView<T, Operation>* child);
	~OrderedReducerView();

	OrderedReducerView<T, Operation>* fold();
	OrderedReducerView<T, Operation>* create_parent_view();
	void reduce();
	bool is_reduced();
	void notify_parent();
	void set_finished();
	void add_data(T data);
	T get_data();
private:
	Operation reduce;

	T data;
	OrderedReducerView<T, Operation>* pred;
	OrderedReducerView<T, Operation>* parent;
	OrderedReducerView<T, Operation>* reuse;
	// Bit 0: finished, Bit 1: has unfinished child, Bit 2: has been used
	uint8_t state;
};

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>::OrderedReducerView(OrderedReducerView<T, Operation>* pred, OrderedReducerView<T, Operation>* child)
:data(reduce.get_identity()), pred(pred), parent(NULL), reuse(NULL), state(child != NULL?0x2:0x0) {
	assert(pred == NULL || child == NULL);
	if(child != NULL) {
		child->parent = this;
	}
}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>::~OrderedReducerView() {
	assert((state & 0x3) == 0x1);

	if(reuse != NULL) {
		delete reuse;
	}
}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>* OrderedReducerView<T, Operation>::fold() {
	OrderedReducerView<T, Operation>* ret = this;
	while((ret->state & 0x4) == 0 && ret->pred != NULL && (ret->pred->state & 0x1) == 0x1) {
		ret->pred->reuse = ret;
		ret = ret->pred;
	}
	return ret;
}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>* OrderedReducerView<T, Operation>::create_parent_view() {
	OrderedReducerView<T, Operation>* ret = reuse;
	if(ret != NULL) {
		reuse = ret->reuse;
		ret->data = reduce.get_identity();
		ret->pred = NULL;
		ret->parent = NULL;
		ret->reuse = NULL;
		ret->state = 0x2;
	}
	else {
		ret = new View(NULL, this);
	}
	return ret;
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::reduce() {
	if(pred != NULL) {
		// Definitely not waiting for a child any more
		state &= 0xFD;

		while(pred != NULL && (pred->state & 0x3) == 0x1) {
			data = reduce(pred->data, data);
			// TODO: put into memory reclamation instead
			delete pred;
		}
	}
}

template <typename T, template <typename S> class Operation>
bool OrderedReducerView<T, Operation>::is_reduced() {
	return pred == NULL && (state & 0x2) == 0;
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::notify_parent() {
	if(parent != NULL) {
		parent->pred = this;
	}
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::set_finished() {
	if(pred != NULL) {
		state = (state & 0xFD) | 0x1;
	}
	else {
		state |= 0x1;
	}
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::add_data(T data) {
	state |= 0x4;
	this->data = Operation(this->data, data);
}

template <typename T, template <typename S> class Operation>
T OrderedReducerView<T, Operation>::get_data() {
	return data;
}

}

#endif /* ORDEREDREDUCERVIEW_H_ */
