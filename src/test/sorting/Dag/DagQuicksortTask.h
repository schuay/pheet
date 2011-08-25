/*
 * DagQuicksortTask.h
 *
 *  Created on: 08.04.2011
 *      Author: Martin Wimmer
 *     License: Pheet license
 */

#ifndef DAGQUICKSORTTASK_H_
#define DAGQUICKSORTTASK_H_

#include <functional>
#include <algorithm>

namespace pheet {

template <class Task, size_t CUTOFF_LENGTH = 512>
class DagQuicksortTask : public Task {
public:
	DagQuicksortTask(unsigned int* data, size_t length);
	virtual ~DagQuicksortTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	unsigned int* data;
	size_t length;
};

template <class Task, size_t CUTOFF_LENGTH>
DagQuicksortTask<Task, CUTOFF_LENGTH>::DagQuicksortTask(unsigned int* data, size_t length)
: data(data), length(length) {

}

template <class Task, size_t CUTOFF_LENGTH>
DagQuicksortTask<Task, CUTOFF_LENGTH>::~DagQuicksortTask() {

}

template <class Task, size_t CUTOFF_LENGTH>
void DagQuicksortTask<Task, CUTOFF_LENGTH>::operator()(typename Task::TEC& tec) {
	if(length <= 1)
		return;

	unsigned int * middle = std::partition(data, data + length - 1,
		std::bind2nd(std::less<unsigned int>(), *(data + length - 1)));
	size_t pivot = middle - data;
	std::swap(*(data + length - 1), *middle);    // move pivot to middle

	if(pivot > CUTOFF_LENGTH) {
		tec.template spawn<DagQuicksortTask<Task, CUTOFF_LENGTH> >(data, pivot);
	}
	else {
		tec.template call<DagQuicksortTask<Task, CUTOFF_LENGTH>>(data, pivot);
	}
	tec.template call<DagQuicksortTask<Task, CUTOFF_LENGTH>>(data + pivot + 1, length - pivot - 1);
}

}

#endif /* DAGQUICKSORTTASK_H_ */
