/*
 * StrategySchedulerTaskDescriptor.h
 *
 *  Created on: Mar 8, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef PRIORITYSCHEDULER2TASKDESCRIPTOR_H_
#define PRIORITYSCHEDULER2TASKDESCRIPTOR_H_

namespace pheet {

template <class Pheet>
class StrategySchedulerTaskDescriptor {
public:
	typedef Pheet::Environment::PlaceDescriptor PlaceDesc;
	typedef Pheet::Environment::Place Place;
	typedef StrategySchedulerTaskDescriptor<Pheet> Self;

	StrategySchedulerTaskDescriptor();
	~StrategySchedulerTaskDescriptor();

//	PlaceDesc& get_place_desc();
	procs_t get_distance();
	bool is_newer(Self const& other);

private:

//	PlaceDesc& place;
};

template <class Pheet>
StrategySchedulerTaskDescriptor<Pheet>::StrategySchedulerTaskDescriptor(PlaceDesc& place)
: place(place) {

}

template <class Pheet>
StrategySchedulerTaskDescriptor<Pheet>::~StrategySchedulerTaskDescriptor() {

}
/*
template <class Pheet>
PlaceDesc& StrategySchedulerTaskDescriptor<Pheet>::get_place_desc() {
	return place;
}*/

template <class Pheet>
bool StrategySchedulerTaskDescriptor<Pheet>::is_newer(Self const& other) {

}

}

#endif /* PRIORITYSCHEDULER2TASKDESCRIPTOR_H_ */
