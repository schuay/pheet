/*
 * EventsList.h
 *
 *  Created on: 22 jun 2012
 *      Author: Daniel Cederman
 */

#ifndef EVENTSLIST_H_
#define EVENTSLIST_H_

#include "../../Reducer/List/ListReducer.h"

namespace pheet {


  template <typename E>
    class Event
    {
      struct timeval start;
      E value;
    public:
    Event(struct timeval start, E value):start(start),value(value)
      {}
      void print(struct timeval expstart)
      {
	double time = (start.tv_sec - expstart.tv_sec) + 1.0e-6 * start.tv_usec - 1.0e-6 * expstart.tv_usec;

	std::cout << time << ": " << value << std::endl;
      }
    };

  template <class Pheet, typename E, bool enabled>
    class EventsList;


  template <class Pheet, typename E>
    class EventsList<Pheet, E, true>
    {
      ListReducer<Pheet, Event<E> > events;
      struct timeval start;
    public:
      EventsList()
	{
	  gettimeofday(&start,0);
	}

      inline EventsList(EventsList<Pheet, E, true> const& other):events(other.events)
      {

      }

      void add(E const& value)
      {
	struct timeval currtime;
	gettimeofday(&currtime,0);

	Event<E> e(currtime,value);
	events.add(e);
      }

      void print()
      {
	std::vector<Event<E> > eventslist = events.get_list();
	for(size_t i=0;i<eventslist.size();i++)
	  eventslist[i].print(start);
      }

    };

  template <class Pheet, typename E>
    class EventsList<Pheet, E, false>
    {
    public:
      EventsList()
	{
	}

      inline EventsList(EventsList<Pheet, E, false> const&)
      {

      }

      void add(E const&)
      {
      }

      void print() {}

    };

  template <class Pheet, typename E, bool enabled>
  using EventsListPerformanceCounter = EventsList<Pheet, E, enabled>;
}

#endif /* EVENTSLIST_H_ */
