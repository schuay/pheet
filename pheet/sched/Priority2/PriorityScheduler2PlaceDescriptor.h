/*
 * PriorityScheduler2PlaceDescriptor.h
 *
 *  Created on: Mar 8, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_
#define PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_

namespace pheet {

template <class Pheet>
class PriorityScheduler2PlaceDescriptor {
public:
	typedef typename Pheet::Scheduler::Place Place;
	typedef PriorityScheduler2PlaceDescriptor<Pheet> Self;

	PriorityScheduler2PlaceDescriptor(Place* place);
	~PriorityScheduler2PlaceDescriptor();

	procs_t get_distance(Self& other);
	procs_t get_id();

private:
	Place* place;
};

template <class Pheet>
PriorityScheduler2PlaceDescriptor<Pheet>::PriorityScheduler2PlaceDescriptor(Place* place)
: place(place) {

}

template <class Pheet>
PriorityScheduler2PlaceDescriptor<Pheet>::~PriorityScheduler2PlaceDescriptor() {

}

template <class Pheet>
procs_t PriorityScheduler2PlaceDescriptor<Pheet>::get_distance(Self& other) {
	return place->get_distance(other.place);
}

template <class Pheet>
procs_t PriorityScheduler2PlaceDescriptor<Pheet>::get_id() {
	return place->get_id();
}

#endif /* PRIORITYSCHEDULER2PLACEDESCRIPTOR_H_ */
