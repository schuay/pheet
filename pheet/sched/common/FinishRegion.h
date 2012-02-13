/*
 * Finish.h
 *
 *  Created on: 26.07.2011
 *   Author(s): Martin Wimmer
 *     License: Pheet license
 */

#ifndef FINISH_H_
#define FINISH_H_

/*
 *
 */
namespace pheet {

template <class Sched>
class FinishRegion {
public:
	typedef Sched Scheduler;

	FinishRegion(typename Scheduler::TaskExecutionContext& tec);
	~FinishRegion();

private:
	typename Scheduler::TaskExecutionContext& tec;
};

template <class Sched>
FinishRegion<Sched>::FinishRegion(typename Scheduler::TaskExecutionContext& tec)
	:tec(tec) {
	tec.start_finish_region();
}

template <class Sched>
FinishRegion<Sched>::~FinishRegion() {
	tec.end_finish_region();
}

}

#endif /* FINISH_H_ */
