/*
 * OrderedReducerView.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef ORDEREDREDUCERVIEW_H_
#define ORDEREDREDUCERVIEW_H_

#include "../../../settings.h"
#include "../../../misc/atomics.h"
#include "../../../misc/types.h"

#include <stdint.h>
#include <assert.h>

/*
 *
 */
namespace pheet {

template <class Monoid>
class OrderedReducerView {
public:
	template <typename ... ConsParams>
	OrderedReducerView(ConsParams&& ... params);
	OrderedReducerView(Monoid const& template_monoid);
	~OrderedReducerView();

	OrderedReducerView<Monoid>* fold();
	OrderedReducerView<Monoid>* create_parent_view();
	void reduce();
	bool is_reduced();
	void set_finished();
	void set_predecessor(OrderedReducerView<Monoid>* pred);
	template <typename ... PutParams>
	void add_data(PutParams&& ... params);
	typename Monoid::OutputType get_data();
private:
	typedef OrderedReducerView<Monoid> View;

	Monoid data;
	OrderedReducerView<Monoid>* pred;
	OrderedReducerView<Monoid>* reuse;
	// Bit 1: has unfinished child, Bit 2: has been used (Bit 0 isn't used any more)
	uint8_t state;
};

template <class Monoid>
template <typename ... ConsParams>
OrderedReducerView<Monoid>::OrderedReducerView(ConsParams&& ... params)
:data(static_cast<ConsParams&&>(params) ...), pred(NULL), reuse(NULL), state(0x0u) {

}

template <class Monoid>
OrderedReducerView<Monoid>::OrderedReducerView(Monoid const& template_monoid)
:data(template_monoid), pred(NULL), reuse(NULL), state(0x2u) {

}

template <class Monoid>
OrderedReducerView<Monoid>::~OrderedReducerView() {
	if(reuse != NULL) {
		delete reuse;
	}
}

template <class Monoid>
OrderedReducerView<Monoid>* OrderedReducerView<Monoid>::fold() {
	OrderedReducerView<Monoid>* ret = this;
	while((ret->state & 0x4) == 0 && ret->pred != NULL) {
		OrderedReducerView<Monoid>* tmp = ret->pred;
		while(tmp->reuse != NULL) {
			tmp = tmp->reuse;
		}
		tmp->reuse = ret;
		ret = ret->pred;
	}
	if(ret->pred != NULL) {
		ret->pred->reduce();
	}
	return ret;
}

template <class Monoid>
OrderedReducerView<Monoid>* OrderedReducerView<Monoid>::create_parent_view() {
	OrderedReducerView<Monoid>* ret = reuse;
	if(ret != NULL) {
		reuse = ret->reuse;
		ret->data.reset();
		ret->pred = NULL;
	//	ret->parent = NULL;
		ret->reuse = NULL;
		ret->state = 0x2;
	}
	else {
		ret = new View(static_cast<Monoid const&>(data));
		assert(ret->state == 0x2);
	}
	return ret;
}

template <class Monoid>
void OrderedReducerView<Monoid>::reduce() {
	if(pred != NULL) {
		// Definitely not waiting for a child any more
		state &= 0xFD;

		if((pred->state & 0x2) == 0x0) {
			do {
				data.left_reduce(pred->data);
				View* tmp = pred->pred;
				// No memory reclamation as this view is from another fork
				delete pred;
				pred = tmp;
			}while(pred != NULL && ((pred->state & 0x2) == 0x0 || pred->pred != NULL));

			// The pred pointer is the only dangerous pointer, as the parent might read an old value while folding/reducing
			// This means we need a fence. The good thing is that reduction only happens when there was a
			// fork previously, so this fence should be rare
			MEMORY_FENCE();
		}
	}
}

template <class Monoid>
bool OrderedReducerView<Monoid>::is_reduced() {
	return pred == NULL && (state & 0x2) == 0;
}

template <class Monoid>
void OrderedReducerView<Monoid>::set_predecessor(OrderedReducerView<Monoid>* pred) {
	assert(this->pred == NULL);
	assert(pred != this);
	this->pred = pred;
}

template <typename Monoid>
template <typename ... PutParams>
void OrderedReducerView<Monoid>::add_data(PutParams&& ... params) {
	state |= 0x4;
	data.put(static_cast<PutParams&&>(params) ...);
}

template <class Monoid>
typename Monoid::OutputType OrderedReducerView<Monoid>::get_data() {
	return data.get();
}

}

#endif /* ORDEREDREDUCERVIEW_H_ */
