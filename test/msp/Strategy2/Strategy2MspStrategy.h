/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGY2MSPSTRATEGY_H_
#define STRATEGY2MSPSTRATEGY_H_

#include "lib/ShortestPath/Path.h"
#include "Less.h"
#include "pheet/ds/StrategyTaskStorage/ParetoLocality/PriorityVector.h"

#define NR_DIMENSIONS (3)

namespace pheet
{

template <class Pheet, template <class, class> class TaskStorageT>
class Strategy2MspStrategy
	: public Pheet::Environment::BaseStrategy
{
public:
	typedef Strategy2MspStrategy<Pheet, TaskStorageT> Self;
	typedef typename Pheet::Environment::BaseStrategy BaseStrategy;
	typedef TaskStorageT<Pheet, Self> TaskStorage;
	typedef typename TaskStorage::Place TaskStoragePlace;
	typedef typename Pheet::Place Place;

	Strategy2MspStrategy();
	Strategy2MspStrategy(sp::PathPtr path);
	Strategy2MspStrategy(Self& other);
	Strategy2MspStrategy(Self&& other);

	inline bool prioritize(Self& other) const;

	/**
	 * Returns true if this at dimension dim this has less priority than
	 * other_val
	 */
	inline bool less_priority(size_t dim, size_t other_val) const;

	/**
	 * Returns true if this at dimension dim this has greater priority than
	 * other_val
	 */
	inline bool greater_priority(size_t dim, size_t other_val) const;

	/**
	 * Returns true if this at dimension dim this has equal priority as
	 * other_val
	 */
	inline bool equal_priority(size_t dim, size_t other_val) const;

	inline size_t priority_at(size_t dimension) const;
	inline size_t nr_dimensions() const;


	inline bool dead_task();
	inline bool can_call(TaskStoragePlace*);

	Self& operator=(Self && other);

private:
	sp::PathPtr path;
	less<NR_DIMENSIONS> pareto_less;
	PriorityVector<NR_DIMENSIONS>* w;
};

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy()
{
}

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(sp::PathPtr path)
	: path(path)
{
	w = new PriorityVector<NR_DIMENSIONS>(path);
}

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(Self& other)
	: BaseStrategy(other),
	  path(other.path)
{
	w = new PriorityVector<NR_DIMENSIONS>(path);
}

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(Self&& other)
	: BaseStrategy(other),
	  path(other.path)
{
	w = new PriorityVector<NR_DIMENSIONS>(path);
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
prioritize(Self& other) const
{
	return pareto_less(this->w, other.w);
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
less_priority(size_t dim, size_t other_val) const
{
	assert(dim < w->dimensions());
	//TODO: < should be specified somewhere
	return w->at(dim) > other_val;
}
template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
greater_priority(size_t dim, size_t other_val) const
{
	assert(dim < w->dimensions());
	return w->at(dim) < other_val;
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
equal_priority(size_t dim, size_t other_val) const
{
	return !less_priority(dim, other_val) && !greater_priority(dim, other_val);
}

template <class Pheet, template <class, class> class TaskStorageT>
inline size_t
Strategy2MspStrategy<Pheet, TaskStorageT>::
priority_at(size_t dimension) const
{
	assert(dimension < w->dimensions());
	return w->at(dimension);
}

template <class Pheet, template <class, class> class TaskStorageT>
inline size_t
Strategy2MspStrategy<Pheet, TaskStorageT>::nr_dimensions() const
{
	return NR_DIMENSIONS;
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
dead_task()
{
	return path->dominated();
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
can_call(TaskStoragePlace*)
{
	return false;
}

template <class Pheet, template <class, class> class TaskStorageT>
typename Strategy2MspStrategy<Pheet, TaskStorageT>::Self&
Strategy2MspStrategy<Pheet, TaskStorageT>::
operator=(Strategy2MspStrategy<Pheet, TaskStorageT>::Self && other)
{
	path = other.path;
	w = other.w;
	return *this;
}


} /* namespace pheet */

#endif /* STRATEGY2MSPSTRATEGY_H_ */

