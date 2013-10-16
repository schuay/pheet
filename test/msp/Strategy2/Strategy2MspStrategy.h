/*
 * Copyright Jakob Gruber 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef STRATEGY2MSPSTRATEGY_H_
#define STRATEGY2MSPSTRATEGY_H_

#include "lib/ShortestPath/Path.h"

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

	Strategy2MspStrategy(sp::PathPtr path);
	Strategy2MspStrategy(Self& other);
	Strategy2MspStrategy(Self&& other);

	inline bool prioritize(Self& other) const;
	inline bool dead_task();
	inline bool can_call(TaskStoragePlace*);

private:
	sp::PathPtr path;
};

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(sp::PathPtr path)
	: path(path)
{
}

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(Self& other)
	: BaseStrategy(other),
	  path(other.path)
{
}

template <class Pheet, template <class, class> class TaskStorageT>
Strategy2MspStrategy<Pheet, TaskStorageT>::
Strategy2MspStrategy(Self&& other)
	: BaseStrategy(other),
	  path(other.path)
{
}

template <class Pheet, template <class, class> class TaskStorageT>
inline bool
Strategy2MspStrategy<Pheet, TaskStorageT>::
prioritize(Self& other) const
{
	return (path->weight_sum() < other.path->weight_sum());
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


} /* namespace pheet */

#endif /* STRATEGY2MSPSTRATEGY_H_ */

