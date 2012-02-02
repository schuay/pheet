/*
 * SchedulerTask.h
 *
 *  Created on: 25.07.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef SCHEDULERTASK_H_
#define SCHEDULERTASK_H_

/*
 *
 */
namespace pheet {

template <class Sched>
class SchedulerTask {
public:
	typedef Sched Scheduler;
	typedef typename Scheduler::Finish Finish;
	typedef typename Scheduler::Place Place;
	typedef typename Scheduler::Place TEC;

	SchedulerTask();
	virtual ~SchedulerTask();

	virtual void operator()(typename Scheduler::Place& tec) = 0;

private:
};

template <class Sched>
SchedulerTask<Sched>::SchedulerTask() {

}

template <class Sched>
SchedulerTask<Sched>::~SchedulerTask() {

}

}

#endif /* SCHEDULERTASK_H_ */
