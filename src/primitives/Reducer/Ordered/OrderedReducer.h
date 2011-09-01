/*
 * OrderedReducer.h
 *
 *  Created on: 09.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef ORDEREDREDUCER_H_
#define ORDEREDREDUCER_H_

#include "../../../settings.h"
#include "OrderedReducerView.h"
#include "../../Backoff/Exponential/ExponentialBackoff.h"

/*
 *
 */
namespace pheet {

template <typename T, template <typename S> class Operation>
class OrderedReducer {
public:
	OrderedReducer();
	OrderedReducer(OrderedReducer& other);
	~OrderedReducer();

	void bind(OrderedReducer& other);
	void add_data(T data);
	T get_data();
private:
	void init();
	void finalize();

	typedef OrderedReducerView<T, Operation> View;
	typedef ExponentialBackoff<> Backoff;
	View* my_view;
	View* parent_view;
//	bool root;
};

template <typename T, template <typename S> class Operation>
OrderedReducer<T, Operation>::OrderedReducer()
:my_view(NULL), parent_view(NULL)
{
	my_view = NULL;
	parent_view = NULL;
}

template <typename T, template <typename S> class Operation>
OrderedReducer<T, Operation>::OrderedReducer(OrderedReducer& other) {
	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <typename T, template <typename S> class Operation>
OrderedReducer<T, Operation>::~OrderedReducer() {
	finalize();
}

template <typename T, template <typename S> class Operation>
void OrderedReducer<T, Operation>::bind(OrderedReducer& other) {
	finalize();

	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <typename T, template <typename S> class Operation>
void OrderedReducer<T, Operation>::init() {
	my_view = new View(false);
}

template <typename T, template <typename S> class Operation>
void OrderedReducer<T, Operation>::finalize() {
	if(my_view != NULL) {
		my_view->reduce();
	//	my_view->notify_parent();
		if(parent_view == NULL) {
			assert(my_view->is_reduced());
			delete my_view;
		}
		else {
			// Fence in reduce method (only executed if necessary) ensures that parent gets a consistent view

			// Order between those two operations is not relevant as both have to be finished before we proceed
			// Notify parent view
			parent_view->set_predecessor(my_view);
		}
	}
}

template <typename T, template <typename S> class Operation>
void OrderedReducer<T, Operation>::add_data(T data) {
	if(my_view == NULL) {
		init();
	}
	my_view = my_view->fold();
	my_view->add_data(data);
}

template <typename T, template <typename S> class Operation>
T OrderedReducer<T, Operation>::get_data() {
	if(my_view == NULL) {
		Operation<T> reduce_op;
		return reduce_op.get_identity();
	}
	Backoff bo;
	if(!my_view->is_reduced()) {
		while(true) {
			my_view = my_view->fold();
			my_view->reduce();
			if(my_view->is_reduced()) {
				break;
			}
			bo.backoff();
		}
	}
	return my_view->get_data();
}

}

#endif /* ORDEREDREDUCER_H_ */
