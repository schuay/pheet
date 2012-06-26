/*
 * ListReducer.h
 *
 *  Created on: 10.08.2011
 *   Author(s): Martin Wimmer
 *     License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef LISTREDUCER_H_
#define LISTREDUCER_H_

#include "../Ordered/OrderedReducer.h"
#include "../Ordered/ScalarMonoid.h"
#include "ListOperation.h"

/*
 *
 */
namespace pheet {

  template <class Pheet, typename T, typename E, template <typename S> class Op = ListOperation>
class ListReducer {
public:
	ListReducer();
ListReducer(ListReducer<Pheet, T, E, Op>& other);
ListReducer(ListReducer<Pheet, T,E, Op>&& other);
	~ListReducer();

	void add(E const& value);

	T const& get_list();
private:
	typedef OrderedReducer<Pheet, ScalarMonoid<T, Op> > Reducer;
	Reducer reducer;
};

  template <class Pheet, typename T, typename E, template <typename S> class Op>
  ListReducer<Pheet, T,E, Op>::ListReducer() {

}

  template <class Pheet, typename T, typename E, template <typename S> class Op>
    ListReducer<Pheet, T,E, Op>::ListReducer(ListReducer<Pheet, T,E, Op>& other)
: reducer(other.reducer) {

}

  template <class Pheet, typename T, typename E, template <typename S> class Op>
    ListReducer<Pheet, T,E, Op>::ListReducer(ListReducer<Pheet, T,E, Op>&& other)
: reducer(std::move(other.reducer)) {

}

  template <class Pheet, typename T, typename E, template <typename S> class Op>
  ListReducer<Pheet, T,E, Op>::~ListReducer() {

}

  template <class Pheet, typename T, typename E, template <typename S> class Op>
  void ListReducer<Pheet, T,E, Op>::add(E const& value) {
  T t;
  t.push_back(value);
	reducer.add_data(t);
}


  template <class Pheet, typename T, typename E, template <typename S> class Op>
  T const& ListReducer<Pheet, T,E, Op>::get_list() {
	return reducer.get_data();
}
}

#endif /* LISTREDUCER_H_ */
