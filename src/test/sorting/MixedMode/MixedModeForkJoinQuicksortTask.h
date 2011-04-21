/*
 * MixedModeForkJoinQuicksortTask.h
 *
 *  Created on: 08.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef MIXEDMODEFORKJOINQUICKSORTTASK_H_
#define MIXEDMODEFORKJOINQUICKSORTTASK_H_

#include <functional>
#include <algorithm>

namespace pheet {

template <class Task, size_t CUTOFF_LENGTH = 512>
class MixedModeForkJoinQuicksortTask : public Task {
public:
	MixedModeForkJoinQuicksortTask(unsigned int* data, size_t length);
	virtual ~MixedModeForkJoinQuicksortTask();

	virtual void operator()(typename Task::Scheduler::TaskExecutionContext &tec);

private:
	unsigned int* data;
	size_t length;
};

template <class Task, size_t CUTOFF_LENGTH>
MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH>::MixedModeForkJoinQuicksortTask(unsigned int* data, size_t length)
: data(data), length(length) {

}

template <class Task, size_t CUTOFF_LENGTH>
MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH>::~MixedModeForkJoinQuicksortTask() {

}

template <class Task, size_t CUTOFF_LENGTH>
void MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH>::operator()(typename Task::Scheduler::TaskExecutionContext &tec) {
	if(length <= 1)
		return;

	unsigned int * middle = std::partition(data, data + length - 1,
		std::bind2nd(std::less<unsigned int>(), *(data + length - 1)));
	size_t pivot = middle - data;
	std::swap(*(data + length - 1), *middle);    // move pivot to middle

	if(pivot > CUTOFF_LENGTH) {
		tec.template spawn<MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH> >(data, pivot);
	}
	else {
		tec.template call<MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH>>(data, pivot);
	}
	tec.template call<MixedModeForkJoinQuicksortTask<Task, CUTOFF_LENGTH>>(data + pivot + 1, length - pivot - 1);
}

}

#endif /* MIXEDMODEFORKJOINQUICKSORTTASK_H_ */
