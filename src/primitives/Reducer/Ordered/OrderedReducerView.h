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
	OrderedReducerView(bool is_parent);
	~OrderedReducerView();

	OrderedReducerView<T, Operation>* fold();
	OrderedReducerView<T, Operation>* create_parent_view();
	void reduce();
	bool is_reduced();
//	void notify_parent();
	void set_finished();
	void set_predecessor(OrderedReducerView<T, Operation>* pred);
	void add_data(T data);
	T get_data();
private:
	typedef OrderedReducerView<T, Operation> View;

	Operation<T> reduce_op;

	T data;
	OrderedReducerView<T, Operation>* pred;
//	OrderedReducerView<T, Operation>* parent;
	OrderedReducerView<T, Operation>* reuse;
	// Bit 1: has unfinished child, Bit 2: has been used (Bit 0 isn't used any more)
	uint8_t state;
};

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>::OrderedReducerView(bool is_parent)
:data(reduce_op.get_identity()), pred(NULL)/*, parent(NULL)*/, reuse(NULL), state(((is_parent)?0x2u:0x0u)) {

}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>::~OrderedReducerView() {
	if(reuse != NULL) {
		delete reuse;
	}
}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>* OrderedReducerView<T, Operation>::fold() {
	OrderedReducerView<T, Operation>* ret = this;
	while((ret->state & 0x4) == 0 && ret->pred != NULL) {
		assert(ret->pred->reuse == NULL);
		ret->pred->reuse = ret;
		ret = ret->pred;
	}
	if(ret->pred != NULL) {
		ret->pred->reduce();
	}
	return ret;
}

template <typename T, template <typename S> class Operation>
OrderedReducerView<T, Operation>* OrderedReducerView<T, Operation>::create_parent_view() {
	OrderedReducerView<T, Operation>* ret = reuse;
	if(ret != NULL) {
		reuse = ret->reuse;
		ret->data = reduce_op.get_identity();
		ret->pred = NULL;
	//	ret->parent = NULL;
		ret->reuse = NULL;
		ret->state = 0x2;
	}
	else {
		ret = new View(true);
	}
	return ret;
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::reduce() {
	if(pred != NULL) {
		// Definitely not waiting for a child any more
		state &= 0xFD;

		if((pred->state & 0x2) == 0x0) {
			do {
				data = reduce_op(pred->data, data);
				View* tmp = pred->pred;
				// No memory reclamation as this view is from another fork
				delete pred;
				pred = tmp;
			}while(pred != NULL && (pred->state & 0x2) == 0x0);

			// The pred pointer is the only dangerous pointer, as the parent might read an old value while folding/reducing
			// This means we need a fence. The good thing is that reduction only happens when there was a
			// fork previously, so this fence should be rare
			MEMORY_FENCE();
		}
	}
}

template <typename T, template <typename S> class Operation>
bool OrderedReducerView<T, Operation>::is_reduced() {
	return pred == NULL && (state & 0x2) == 0;
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::set_predecessor(OrderedReducerView<T, Operation>* pred) {
	assert(this->pred == NULL);
	this->pred = pred;
}

template <typename T, template <typename S> class Operation>
void OrderedReducerView<T, Operation>::add_data(T data) {
	state |= 0x4;
	this->data = reduce_op(this->data, data);
}

template <typename T, template <typename S> class Operation>
T OrderedReducerView<T, Operation>::get_data() {
	return data;
}

}

#endif /* ORDEREDREDUCERVIEW_H_ */
