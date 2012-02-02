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

template <class Pheet, class Monoid>
class OrderedReducer {
public:
	template <typename ... ConsParams>
	OrderedReducer(ConsParams&& ... params);
	OrderedReducer(OrderedReducer& other);
	~OrderedReducer();

	template <typename ... PutParams>
	void add_data(PutParams&& ... params);
	typename Monoid::OutputType get_data();
private:
	void finalize();
	void minimize();

	typedef OrderedReducerView<Monoid> View;
	typedef typename Pheet::Backoff Backoff;
	View* my_view;
	View* parent_view;
	typename Pheet::Place* tec;
};

template <class Pheet, class Monoid>
template <typename ... ConsParams>
OrderedReducer<Pheet, Monoid>::OrderedReducer(ConsParams&& ... params)
:my_view(new View(static_cast<ConsParams&&>(params) ...)), parent_view(NULL), tec(Pheet::get_context())
{

}

template <class Pheet, class Monoid>
OrderedReducer<Pheet, Monoid>::OrderedReducer(OrderedReducer& other)
: tec(Pheet::get_context()){
	// Before we create a new view, we should minimize. Maybe this provides us with a view to reuse
	other.minimize();

	my_view = other.my_view;
	parent_view = my_view->create_parent_view();
	other.my_view = parent_view;
}

template <class Pheet, class Monoid>
OrderedReducer<Pheet, Monoid>::~OrderedReducer() {
	finalize();
}

template <class Pheet, class Monoid>
void OrderedReducer<Pheet, Monoid>::finalize() {
	minimize();
	if(parent_view == NULL) {
		assert(my_view->is_reduced());
		delete my_view;
	}
	else if(tec == Pheet::get_context()) {
		// This reducer has only been used locally. No need for sync
		parent_view->set_local_predecessor(my_view);
	}
	else {
		// Notify parent view
		parent_view->set_predecessor(my_view);
	}
}

/*
 * Do folding and reduce if possible. May free some memory that we can reuse
 */
template <class Pheet, class Monoid>
void OrderedReducer<Pheet, Monoid>::minimize() {
	my_view = my_view->fold();
	my_view->reduce();
}

template <class Pheet, class Monoid>
template <typename ... PutParams>
void OrderedReducer<Pheet, Monoid>::add_data(PutParams&& ... params) {
	my_view = my_view->fold();
	my_view->add_data(static_cast<PutParams&&>(params) ...);
}

template <class Pheet, class Monoid>
typename Monoid::OutputType OrderedReducer<Pheet, Monoid>::get_data() {
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
