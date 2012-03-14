/*
 * StrategySchedulerPlaceDescriptor.h
 *
 *  Created on: Mar 8, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_
#define PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_

namespace pheet {

template <class Pheet>
class StrategySchedulerPlaceDescriptor {
public:
	typedef typename Pheet::Scheduler::Place Place;
	typedef StrategySchedulerPlaceDescriptor<Pheet> Self;

	StrategySchedulerPlaceDescriptor(Place* place);
	~StrategySchedulerPlaceDescriptor();

	procs_t get_distance(Self& other);
	procs_t get_id();

	Place* get_place();
private:
	Place* place;
};

template <class Pheet>
StrategySchedulerPlaceDescriptor<Pheet>::StrategySchedulerPlaceDescriptor(Place* place)
: place(place) {

}

template <class Pheet>
StrategySchedulerPlaceDescriptor<Pheet>::~StrategySchedulerPlaceDescriptor() {

}

template <class Pheet>
procs_t StrategySchedulerPlaceDescriptor<Pheet>::get_distance(Self& other) {
	return place->get_distance(other.place);
}

template <class Pheet>
procs_t StrategySchedulerPlaceDescriptor<Pheet>::get_id() {
	return place->get_id();
}

#endif /* PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_ */
