/*
 * MixedModeForkJoinQuicksortTask.h
 *
 *  Created on: 08.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef MIXEDMODEFORKJOINQUICKSORTTASK_H_
#define MIXEDMODEFORKJOINQUICKSORTTASK_H_

#include <algorithm>

namespace pheet {

template <class Task>
class MixedModeForkJoinQuicksortTask : Task {
public:
	typedef MixedModeForkJoinQuicksortTask<Task> bla;
	MixedModeForkJoinQuicksortTask(unsigned int* data, size_t length);
	virtual ~MixedModeForkJoinQuicksortTask();

	void execute(typename Task::Scheduler::TaskExecutionContext &tec);

private:
	unsigned int* data;
	size_t length;
};

template <class Task>
MixedModeForkJoinQuicksortTask<Task>::MixedModeForkJoinQuicksortTask(unsigned int* data, size_t length)
: data(data), length(length) {

}

template <class Task>
MixedModeForkJoinQuicksortTask<Task>::~MixedModeForkJoinQuicksortTask() {

}

template <class Task>
void MixedModeForkJoinQuicksortTask<Task>::execute(typename Task::Scheduler::TaskExecutionContext &tec) {
	if(length <= 1)
		return;

	unsigned int * middle = std::partition(data, data + length - 1,
		std::bind2nd(std::less<unsigned int>(), *(data + length - 1)));
	size_t pivot = middle - data;
	swap(*(data + length - 1), *middle);    // move pivot to middle

	tec.template spawn<MixedModeForkJoinQuicksortTask>(data, pivot);
	tec.template spawn<MixedModeForkJoinQuicksortTask>(data + pivot + 1, length - pivot - 1);
}

}

#endif /* MIXEDMODEFORKJOINQUICKSORTTASK_H_ */
