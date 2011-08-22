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

	void add_data(T data);
	T get_data();
private:
	typedef OrderedReducerView<T, Operation> View;
	typedef ExponentialBackoff<> Backoff;
	View* my_view;
	View* parent_view;
//	bool root;
};

template <typename T, template <typename S> class Operation>
OrderedReducer<T, Operation>::OrderedReducer() {
	my_view = new View(false);
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
	my_view->reduce();
//	my_view->notify_parent();
	if(parent_view == NULL) {
		assert(my_view->is_reduced());
		delete my_view;
	}
	else {
		// TODO: do we need a fence to make sure reduce has been completed?

		// Order between those two operations is not relevant as both have to be finished before we proceed
		// Notify parent view
		parent_view->set_predecessor(my_view);
		// Finalize this view
		my_view->set_finished();
	}
}

template <typename T, template <typename S> class Operation>
void OrderedReducer<T, Operation>::add_data(T data) {
	my_view = my_view->fold();
	my_view->add_data(data);
}

template <typename T, template <typename S> class Operation>
T OrderedReducer<T, Operation>::get_data() {
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
