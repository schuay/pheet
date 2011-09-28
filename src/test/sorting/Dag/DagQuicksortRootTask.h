/*
 * DagQuicksortRootTask.h
 *
 *  Created on: 28.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#ifndef DAGQUICKSORTROOTTASK_H_
#define DAGQUICKSORTROOTTASK_H_

#include "DagQuicksortTask.h"

/*
 *
 */
namespace pheet {

template <class Task, size_t CUTOFF_LENGTH = 512>
class DagQuicksortRootTask : public Task {
public:
	DagQuicksortRootTask(unsigned int* data, size_t length);
	virtual ~DagQuicksortTask();

	virtual void operator()(typename Task::TEC& tec);

private:
	unsigned int* data;
	size_t length;
};

template <class Task, size_t CUTOFF_LENGTH>
DagQuicksortRootTask<Task, CUTOFF_LENGTH>::DagQuicksortRootTask(unsigned int* data, size_t length)
: data(data), length(length) {

}

template <class Task, size_t CUTOFF_LENGTH>
DagQuicksortRootTask<Task, CUTOFF_LENGTH>::~DagQuicksortRootTask() {

}

template <class Task, size_t CUTOFF_LENGTH>
void DagQuicksortRootTask<Task, CUTOFF_LENGTH>::operator()(typename Task::TEC& tec) {
	tec.template finish<DagQuicksortTask<Task, CUTOFF_LENGTH>>(data + pivot + 1, length - pivot - 1);
}


}

#endif /* DAGQUICKSORTROOTTASK_H_ */
