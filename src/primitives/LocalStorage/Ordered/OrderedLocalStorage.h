/*
 * OrderedLocalStorage.h
 *
 *  Created on: 09.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef ORDEREDLOCALSTORAGE_H_
#define ORDEREDLOCALSTORAGE_H_

#include "../../../settings.h"
#include "OrderedLocalStorageView.h"
#include "../../Backoff/Exponential/ExponentialBackoff.h"

/*
 *
 */
namespace pheet {

template <class T>
class OrderedLocalStorage {
public:
	OrderedLocalStorage();
	OrderedLocalStorage(OrderedLocalStorage& other);
	~OrderedLocalStorage();

	void bind(OrderedLocalStorage& other);
	T& get();
private:
	void init();
	void finalize();

	typedef OrderedLocalStorageView<T, Operation> View;
	typedef ExponentialBackoff<> Backoff;

	View* my_view;
	View* parent_view;
};

template <typename T, template <typename S> class Operation>
OrderedLocalStorage<T, Operation>::OrderedLocalStorage()
:my_view(NULL), parent_view(NULL)
{
	my_view = NULL;
	parent_view = NULL;
}

template <typename T, template <typename S> class Operation>
OrderedLocalStorage<T, Operation>::OrderedLocalStorage(OrderedLocalStorage& other) {
	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <typename T, template <typename S> class Operation>
OrderedLocalStorage<T, Operation>::~OrderedLocalStorage() {
	finalize();
}

template <typename T, template <typename S> class Operation>
void OrderedLocalStorage<T, Operation>::bind(OrderedLocalStorage& other) {
	finalize();

	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <typename T, template <typename S> class Operation>
void OrderedLocalStorage<T, Operation>::init() {
	my_view = new View(false);
}

template <typename T, template <typename S> class Operation>
void OrderedLocalStorage<T, Operation>::finalize() {
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
			// Finalize this view
			my_view->set_finished();
		}
	}
}

template <typename T, template <typename S> class Operation>
T& OrderedLocalStorage<T, Operation>::get_data() {
	if(my_view == NULL) {
		init();
	}
	my_view = my_view->fold();
	return my_view->get_data();
}

}

#endif /* ORDEREDLOCALSTORAGE_H_ */
