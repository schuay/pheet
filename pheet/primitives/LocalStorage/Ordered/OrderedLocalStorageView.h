/*
 * OrderedLocalStorageView.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef ORDEREDLOCALSTORAGEVIEW_H_
#define ORDEREDLOCALSTORAGEVIEW_H_

/*
 *
 */
namespace pheet {

template <typename T>
class OrderedLocalStorageView {
public:
	OrderedLocalStorageView(bool is_parent);
	~OrderedLocalStorageView();

	OrderedLocalStorageView<T>* fold();
	OrderedLocalStorageView<T>* create_parent_view();
	void reduce();
	bool is_reduced();
//	void notify_parent();
	void set_predecessor(OrderedLocalStorageView<T>* pred);

	T& get_data();
private:
	typedef OrderedLocalStorageView<T> View;

	T* data;
	OrderedLocalStorageView<T>* pred;
	OrderedLocalStorageView<T>* reuse;

	// Bit 1: has unfinished child
	uint8_t state;
};

template <typename T>
OrderedLocalStorageView<T>::OrderedLocalStorageView(bool is_parent)
:data(NULL), pred(NULL), reuse(NULL), state(((is_parent)?0x2u:0x0u)) {

}

template <typename T>
OrderedLocalStorageView<T>::~OrderedLocalStorageView() {
	if(data != NULL) {
		delete data;
	}
	if(reuse != NULL) {
		delete reuse;
	}
}

template <typename T>
OrderedLocalStorageView<T>* OrderedLocalStorageView<T>::fold() {
	OrderedLocalStorageView<T>* ret = this;
	while(ret->data == NULL && ret->pred != NULL) {
		assert(ret->pred->reuse == NULL);
		ret->pred->reuse = ret;
		ret = ret->pred;
	}
	if(ret->pred != NULL) {
		ret->pred->reduce();
	}
	return ret;
}

template <typename T>
OrderedLocalStorageView<T>* OrderedLocalStorageView<T>::create_parent_view() {
	OrderedLocalStorageView<T>* ret = reuse;
	if(ret != NULL) {
		reuse = ret->reuse;
		assert(ret->data == NULL);
	//	ret->data = reduce_op.get_identity();
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

template <typename T>
void OrderedLocalStorageView<T>::reduce() {
	if(pred != NULL) {
		// Definitely not waiting for a child any more
		state &= 0xFD;

		do {
			View* tmp = pred->pred;
			// No memory reclamation as this view is from another fork
			delete pred;
			pred = tmp;
		}while(pred != NULL);

		// The pred pointer is the only dangerous pointer, as the parent might read an old value while folding/reducing
		// This means we need a fence. The good thing is that reduction only happens when there was a
		// fork previously, so this fence should be rare
	}
}

template <typename T>
bool OrderedLocalStorageView<T>::is_reduced() {
	return pred == NULL && (state & 0x2) == 0;
}

template <typename T>
void OrderedLocalStorageView<T>::set_predecessor(OrderedLocalStorageView<T>* pred) {
	assert(this->pred == NULL);
	this->pred = pred;
}

template <typename T>
T& OrderedLocalStorageView<T>::get_data() {
	if(data == NULL) {
		data = new T();
	}
	return *data;
}

}

#endif /* ORDEREDLOCALSTORAGEVIEW_H_ */
