/*
 * SchedulerLambdaTask.h
 *
 *  Created on: 25.08.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef SCHEDULERLAMBDATASK_H_
#define SCHEDULERLAMBDATASK_H_

namespace pheet {


/*
 * Is planned to provide support for lambda expressions instead of full tasks.
 * Will never be the preferred way, but might be useful for some applications nonetheless
 */
template <class Task, class Lambda>
class SchedulerLambdaTask : public Task {
public:
	SchedulerLambdaTask(Lambda lambda);
	~SchedulerLambdaTask();

	virtual void operator()(typename Task::Scheduler::TaskExecutionContext &tec);

private:
	Lambda lambda;
};

template <class Task, class Lambda>
SchedulerLambdaTask<Task, Lambda>::SchedulerLambdaTask(Lambda lambda)
:lambda(lamda){

}

template <class Task, class Lambda>
SchedulerLambdaTask<Task, Lambda>::~SchedulerLambdaTask() {

}

template <class Task, class Lambda>
void SchedulerLambdaTask<Task, Lambda>::operator()(typename Task::Scheduler::TaskExecutionContext &tec) {
	lambda(tec);
}

}

#endif /* SCHEDULERLAMBDATASK_H_ */
