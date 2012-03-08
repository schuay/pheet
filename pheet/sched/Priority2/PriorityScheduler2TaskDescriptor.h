/*
 * PriorityScheduler2TaskDescriptor.h
 *
 *  Created on: Mar 8, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2TASKDESCRIPTOR_H_
#define PRIORITYSCHEDULER2TASKDESCRIPTOR_H_

namespace pheet {

template <class Pheet>
class PriorityScheduler2TaskDescriptor {
public:
	typedef Pheet::Environment::PlaceDescriptor PlaceDesc;
	typedef PriorityScheduler2TaskDescriptor<Pheet> Self;

	PriorityScheduler2TaskDescriptor();
	~PriorityScheduler2TaskDescriptor();

	PlaceDesc& get_place_desc();
	bool is_newer(Self& other);

private:

	PlaceDesc& place;
};

template <class Pheet>
PriorityScheduler2TaskDescriptor<Pheet>::PriorityScheduler2TaskDescriptor(PlaceDesc& place)
: place(place) {

}

template <class Pheet>
PriorityScheduler2TaskDescriptor<Pheet>::~PriorityScheduler2TaskDescriptor() {

}

template <class Pheet>
PlaceDesc& PriorityScheduler2TaskDescriptor<Pheet>::get_place_desc() {
	return place;
}

}

#endif /* PRIORITYSCHEDULER2TASKDESCRIPTOR_H_ */
