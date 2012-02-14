/*
 * SchedulerFunctorTask.h
 *
 *  Created on: Feb 14, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef SCHEDULERFUNCTORTASK_H_
#define SCHEDULERFUNCTORTASK_H_

#include <tuple>
#include <functional>

namespace pheet {

template <class Pheet, typename F>
class SchedulerFunctorTask : public Pheet::Environment::Task {
public:
	SchedulerFunctorTask(F& f);
	SchedulerFunctorTask(F&& f);
	virtual ~SchedulerFunctorTask();

	virtual void operator()();

private:
	F f;
};

template <class Pheet, typename F>
SchedulerFunctorTask<Pheet, F>::SchedulerFunctorTask(F& f)
: f(f) {

}

template <class Pheet, typename F>
SchedulerFunctorTask<Pheet, F>::SchedulerFunctorTask(F&& f)
: f(f) {

}

template <class Pheet, typename F>
SchedulerFunctorTask<Pheet, F>::~SchedulerFunctorTask() {

}

template <class Pheet, typename F>
void SchedulerFunctorTask<Pheet, F>::operator()() {
	f();
}

}

#endif /* SCHEDULERFUNCTORTASK_H_ */
