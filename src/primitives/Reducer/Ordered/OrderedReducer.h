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

#include <assert.h>

/*
 *
 */
namespace pheet {

template <class Monoid>
class OrderedReducer {
public:
	template <typename ... ConsParams>
	OrderedReducer(ConsParams&& ... params);
	OrderedReducer(OrderedReducer& other);
	~OrderedReducer();

//	void bind(OrderedReducer& other);

	template <typename ... PutParams>
	void add_data(PutParams&& ... params);
	typename Monoid::OutputType get_data();
private:
//	void init();
	void finalize();

	typedef OrderedReducerView<Monoid> View;
	typedef ExponentialBackoff<> Backoff;
	View* my_view;
	View* parent_view;
//	bool root;
};

template <class Monoid>
template <typename ... ConsParams>
OrderedReducer<Monoid>::OrderedReducer(ConsParams&& ... params)
:my_view(new View(static_cast<ConsParams&&>(params) ...)), parent_view(NULL)
{

}

template <class Monoid>
OrderedReducer<Monoid>::OrderedReducer(OrderedReducer& other) {
	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <class Monoid>
OrderedReducer<Monoid>::~OrderedReducer() {
	finalize();
}
/*
template <class Monoid>
void OrderedReducer<Monoid>::bind(OrderedReducer& other) {
	finalize();

	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}*/
/*
template <class Monoid>
void OrderedReducer<Monoid>::init() {
	my_view = new View(false);
}*/

template <class Monoid>
void OrderedReducer<Monoid>::finalize() {
	my_view->reduce();
//	my_view->notify_parent();
	if(parent_view == NULL) {
		assert(my_view->is_reduced());
		delete my_view;
	}
	else {
		// Fence in reduce method (only executed if necessary) ensures that parent gets a consistent view

		// Notify parent view
		parent_view->set_predecessor(my_view);
	}
}

template <class Monoid>
template <typename ... PutParams>
void OrderedReducer<Monoid>::add_data(PutParams&& ... params) {
	my_view = my_view->fold();
	my_view->add_data(static_cast<PutParams&&>(params) ...);
}

template <class Monoid>
typename Monoid::OutputType OrderedReducer<Monoid>::get_data() {
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
