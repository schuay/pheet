/*
 * BStrategySchedulerTaskDescriptor.h
 *
 *  Created on: Mar 8, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0 (BSL1.0)
 */

#ifndef BSTRATEGYSCHEDULERTASKDESCRIPTOR_H_
#define BSTRATEGYSCHEDULERTASKDESCRIPTOR_H_

namespace pheet {

template <class Pheet, class Strategy>
class BStrategySchedulerTaskDescriptor {
public:
	typedef typename Pheet::Environment::PlaceDescriptor PlaceDesc;
	typedef typename Pheet::Environment::Place Place;
	typedef BStrategySchedulerTaskDescriptor<Pheet> Self;

	BStrategySchedulerTaskDescriptor();
	~BStrategySchedulerTaskDescriptor();

//	PlaceDesc& get_place_desc();
	Strategy& get_strategy();
	procs_t get_distance(Self const& other);
	bool is_newer(Self const& other);

	void configure(Item& item);

private:
	size_t id;
	Place* place;
	Strategy* strategy;

//	PlaceDesc& place;
};

template <class Pheet, class Strategy>
BStrategySchedulerTaskDescriptor<Pheet, Strategy>::BStrategySchedulerTaskDescriptor(size_t id, Place& place, Strategy& strategy)
: id(id), place(&place), strategy(&strategy) {

}

template <class Pheet, class Strategy>
BStrategySchedulerTaskDescriptor<Pheet, Strategy>::~BStrategySchedulerTaskDescriptor() {

}
/*
template <class Pheet>
PlaceDesc& BStrategySchedulerTaskDescriptor<Pheet>::get_place_desc() {
	return place;
}*/

template <class Pheet, class Strategy>
inline Strategy& BStrategySchedulerTaskDescriptor<Pheet, Strategy>::get_strategy() {
	return *strategy;
}

template <class Pheet, class Strategy>
inline procs_t BStrategySchedulerTaskDescriptor<Pheet, Strategy>::get_distance(Self const& other) {
	return place->get_distance(other.place);
}

template <class Pheet, class Strategy>
inline bool BStrategySchedulerTaskDescriptor<Pheet, Strategy>::is_newer(Self const& other) {
	return id > other.id;
}

}

#endif /* BSTRATEGYSCHEDULERTASKDESCRIPTOR_H_ */
