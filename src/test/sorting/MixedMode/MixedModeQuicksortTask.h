/*
 * MixedModeQuicksortTask.h
 *
 *  Created on: 30.07.2011
 *      Author: Martin Wimmer
 *     License: Pheet License
 */

#ifndef MIXEDMODEQUICKSORTTASK_H_
#define MIXEDMODEQUICKSORTTASK_H_

#include "../MixedMode/MixedModeQuicksortTask.h"
#include "../../../primitives/Backoff/Exponential/ExponentialBackoff.h"
#include "../../../primitives/Barrier/Simple/SimpleBarrier.h"

namespace pheet {

template <class Task, size_t BLOCK_SIZE = 4096>
class MixedModeQuicksortTask : public Task {
public:
	typedef StandardExponentialBackoff Backoff;

	MixedModeQuicksortTask(unsigned int* data, size_t length);
	virtual ~MixedModeQuicksortTask();

	virtual void operator()(typename Task::Scheduler::TaskExecutionContext &tec);

private:
	void partition(typename Task::Scheduler::TaskExecutionContext &tec);
	void neutralize(ptrdiff_t &leftPos, ptrdiff_t &leftEnd, ptrdiff_t &rightPos, ptrdiff_t &rightEnd);
	bool is_partitioned();
	void assert_is_partitioned();

	unsigned int* data;
	size_t length;

	unsigned int pivot;
	size_t pivotPosition;

	// Aligned starting positions in the array
	ptrdiff_t leftStart;
	ptrdiff_t rightStart;

	// the leftmost unclaimed blocks after the initial team_size blocks
	ptrdiff_t leftBlock;
	// the rightmost unclaimed blocks after the initial team_size blocks
	ptrdiff_t rightBlock;

	// The number of blocks still to process + the initial blocks (which are not subtracted)
	ptrdiff_t remainingBlocks;
	procs_t threads_finished_left;
	procs_t threads_finished_right;

	ptrdiff_t remainingLeft;
	ptrdiff_t remainingRight;

	SimpleBarrier<Backoff> barrier;
};

template <class Task, size_t BLOCK_SIZE>
MixedModeQuicksortTask<Task, BLOCK_SIZE>::MixedModeQuicksortTask(unsigned int* data, size_t length)
: data(data), length(length) {
	pivotPosition = -1;

	ptrdiff_t pivotIndex = length - 1; //rand() % length;
	pivot = data[pivotIndex];

	// assuming BLOCK_SIZE is a power of two
	ptrdiff_t missalign = (ptrdiff_t)(((size_t)data) & (BLOCK_SIZE - 1));
	leftStart = -missalign;
	ptrdiff_t tmp = length - 2;
	ptrdiff_t rightMissalign = (tmp + missalign) & (BLOCK_SIZE - 1);
	if(rightMissalign == 0)
		rightMissalign = BLOCK_SIZE;
	rightStart = tmp - rightMissalign + BLOCK_SIZE - 1;
	if(rightStart < tmp)
		rightStart += BLOCK_SIZE;
/*	if(rightStart >= (tmp + BLOCK_SIZE))
		cout << "wrong rightstart" << endl;*/
/*	if(((rightStart + 1 - leftStart) % BLOCK_SIZE) != 0)
		cout << "bad alignment" << ((rightStart + 1 - leftStart) % BLOCK_SIZE) << endl;
		*/
	// the first block is always given to the coordinating thread
	leftBlock = 1;
	rightBlock = 1;
	remainingBlocks = ((rightStart + 1 - leftStart) / BLOCK_SIZE) - (2);
	threads_finished_left = 0;
	threads_finished_right = 0;

	remainingLeft = length;
	remainingRight = length;
}

template <class Task, size_t BLOCK_SIZE>
MixedModeQuicksortTask<Task, BLOCK_SIZE>::~MixedModeQuicksortTask() {

}

template <class Task, size_t BLOCK_SIZE>
void MixedModeQuicksortTask<Task, BLOCK_SIZE>::operator()(typename Task::Scheduler::TaskExecutionContext &tec) {
	procs_t team_size = tec.get_team_size();
	if(team_size == 1) {
		// For np == 1 switch to dag quicksort
		tec.template call<DagQuicksortTask<Task>>(data, length);
		return;
	}

	partition(tec);

	barrier.barrier(0, team_size);
	MEMORY_FENCE();
	assert_is_partitioned();
	size_t len = pivotPosition;
	procs_t procs = min((len * team_size) / length, ((len / BLOCK_SIZE) / 8) + 1);
	if(procs == 0) {
		procs = 1;
	}
	assert(procs == 1 || (len / procs) > BLOCK_SIZE * 2);
	tec.template spawn_nt<MixedModeQuicksortTask<Task, BLOCK_SIZE> >(procs, data, len);
	len = length - pivotPosition - 1;
	procs = min(team_size - procs, ((len / BLOCK_SIZE) / 8) + 1);
	if(procs == 0) {
		procs = 1;
	}
	assert(procs == 1 || (len / procs) > BLOCK_SIZE * 2);
	tec.template spawn_nt<MixedModeQuicksortTask<Task, BLOCK_SIZE>>(procs, data + pivotPosition + 1, len);
}

template <class Task, size_t BLOCK_SIZE>
void MixedModeQuicksortTask<Task, BLOCK_SIZE>::partition(typename Task::Scheduler::TaskExecutionContext &tec) {
	procs_t localId = tec.get_local_id();
	procs_t team_size = tec.get_team_size();

	ptrdiff_t localLeftBlock = localId;
	ptrdiff_t localRightBlock = localId;

	ptrdiff_t leftPos;
	ptrdiff_t rightPos;
	ptrdiff_t leftEnd;
	ptrdiff_t rightEnd;
	if(tec.is_coordinator()) {
		// Special case for corner blocks, as they might not have the standard block size
		leftPos = 0;
		rightPos = length - 2;
		leftEnd = leftStart + BLOCK_SIZE;
		rightEnd = rightStart - BLOCK_SIZE;
	}
	else {
		// No block for those
		leftPos = 0;
		rightPos = 0;
		leftEnd = 0;
		rightEnd = 0;
	/*	leftPos = leftStart + localLeftBlock * BLOCK_SIZE;
		rightPos = rightStart - localRightBlock * BLOCK_SIZE;
		leftEnd = leftPos + BLOCK_SIZE;
		rightEnd = rightPos - BLOCK_SIZE;*/
	}

	bool standardLoop = true;
	while(standardLoop) {
		neutralize(leftPos, leftEnd, rightPos, rightEnd);

		if(leftPos == leftEnd) {
			int tmp = INT_FETCH_AND_SUB(&remainingBlocks, 1);
			if(tmp > 0) {
				localLeftBlock = INT_FETCH_AND_ADD(&leftBlock, 1);
				leftPos = leftStart + localLeftBlock * BLOCK_SIZE;
				leftEnd = leftPos + BLOCK_SIZE;
#ifdef PARTASK_DEBUG_MODE
				if(leftPos > rightPos) {
					cout << "out of bounds in partitioning" << endl;
					throw -1;
				}
#endif
			//	cout << localId << " new leftEnd " << leftEnd << endl;
			}
			else {
				INT_ATOMIC_ADD(&threads_finished_left, 1);
				standardLoop = false;
			}
		}
		if(rightPos == rightEnd) {
			int tmp = INT_FETCH_AND_SUB(&remainingBlocks, 1);
			if(tmp > 0) {
				localRightBlock = INT_FETCH_AND_ADD(&rightBlock, 1);
				rightPos = rightStart - localRightBlock * BLOCK_SIZE;
				rightEnd = rightPos - BLOCK_SIZE;
			//	cout << localId << " new rightEnd " << rightEnd << endl;
#ifdef PARTASK_DEBUG_MODE
				if(leftPos > rightPos) {
					cout << "out of bounds in partitioning" << endl;
					throw -1;
				}
#endif
			}
			else {
				INT_ATOMIC_ADD(&threads_finished_right, 1);
				standardLoop = false;
			}
		}
	}

	// Now we need the complete team
	tec.sync_team();

	Backoff bo;
	while(true) {
		if(leftPos == leftEnd) {
			if((team_size - threads_finished_left) <= (unsigned)(localId)) {
				// We have finished execution. Do some cleanup
				if(localId == 0) {
				//	leftEnd = rightStart + 1;
					leftPos = leftStart + leftBlock * BLOCK_SIZE;
					assert(leftPos >= 0);
					if(rightPos == rightEnd) {
						// Would be incremented a second time for the same block. Therefore make sure this doesn't happen
						INT_ATOMIC_SUB(&threads_finished_right, 1);
					}
					while(true) {
						while(rightPos > rightEnd) {
							if(data[rightPos] < pivot)
								break;
							rightPos--;
						}
						if(rightPos > rightEnd) {
							while(leftPos < rightPos) {
								if(data[leftPos] > pivot)
									break;
								leftPos++;
							}
						}
						if(leftPos >= rightPos || rightPos == rightEnd) {
							INT_ATOMIC_ADD(&threads_finished_right, 1);

							while(threads_finished_right != team_size) {
								rightPos = remainingRight;
								if(rightPos != (ptrdiff_t)length) {
									remainingRight = (ptrdiff_t)length;
									rightEnd = (rightPos - ((rightPos - leftStart) % BLOCK_SIZE)) - 1;
									break;
								}
								else {
									bo.backoff();
								}
							}
							if(threads_finished_right == team_size)
								break;
						}
						else {
						/*	if(leftPos >= rightPos || leftPos < 0 || rightPos >= (length - 1)) {
								cout << "argh " << endl;
								throw -1;
							}*/
							swap(data[leftPos], data[rightPos]);
						}
					}
				}
				else if(rightPos != rightEnd) {
					while(true) {
						if(remainingRight == (ptrdiff_t)length && (PTRDIFFT_CAS(&(remainingRight), (ptrdiff_t)length, rightPos))) {
					//		cout << "Thread " << localId << " stored " << rightPos << " in remainingright" << endl;
							break;
						}
						else {
							bo.backoff();
						}
					}
				}
				break;
			}
			else {
				ptrdiff_t tmp = remainingLeft;
				if(tmp != (ptrdiff_t)length) {
					if((PTRDIFFT_CAS(&(remainingLeft), tmp, length))) {
						leftPos = tmp;
						leftEnd = leftPos - ((leftPos - leftStart) % BLOCK_SIZE) + BLOCK_SIZE;
				/*		if(leftPos > leftEnd)
							cout << "bah" << endl;*/
					}
					else {
						bo.backoff();
					}
				}
				else
					bo.backoff();
			}
		}
		else if(rightPos == rightEnd) {
			if(team_size - threads_finished_right <= (unsigned)(localId)) {
				// We have finished execution. Do some cleanup
				if(localId == 0) {
				//	leftEnd = rightStart + 1;
					rightPos = rightStart - rightBlock * BLOCK_SIZE;
					assert(rightPos <= (((ptrdiff_t)length) - 2));

					if(leftPos == leftEnd) {
						// Would be incremented a second time for the same block. Therefore make sure this doesn't happen
						INT_ATOMIC_SUB(&threads_finished_left, 1);
					}
					while(true) {
						while(leftPos < leftEnd) {
							if(data[leftPos] > pivot)
								break;
							leftPos++;
						}
						if(leftPos < leftEnd) {
							while(leftPos < rightPos) {
								if(data[rightPos] < pivot)
									break;
								rightPos--;
							}
						}
						if(leftPos >= rightPos || leftPos == leftEnd) {
							INT_ATOMIC_ADD(&threads_finished_left, 1);

							while(threads_finished_left != team_size) {
								leftPos = remainingLeft;
								if(leftPos != (ptrdiff_t)length) {
									remainingLeft = length;
									leftEnd = leftPos - ((leftPos - leftStart) % BLOCK_SIZE) + BLOCK_SIZE;
									break;
								}
								else {
									bo.backoff();
								}
							}

							if(threads_finished_left == team_size)
								break;
						}
						else {
						/*	if(leftPos >= rightPos || leftPos < 0 || rightPos >= (length - 1)) {
								cout << "argh " << endl;
								throw -1;
							}*/
							swap(data[leftPos], data[rightPos]);
						}
					}
				}
				else if(leftPos != leftEnd) {
					while(true) {
						if(remainingLeft == (ptrdiff_t)length && (PTRDIFFT_CAS(&(remainingLeft), (ptrdiff_t)length, leftPos))) {
						//	cout << "Thread " << localId << " stored " << leftPos << " in remainingLeft" << endl;
							break;
						}
						else {
							bo.backoff();
						}
					}
				}
				break;
			}
			else {
				ptrdiff_t tmp = remainingRight;
				if(tmp != (ptrdiff_t)length) {
					if((PTRDIFFT_CAS(&(remainingRight), tmp, length))) {
						rightPos = tmp;
						rightEnd = (rightPos - ((rightPos - leftStart) % BLOCK_SIZE)) - 1;
					}
					else {
						bo.backoff();
					}
				}
				else
					bo.backoff();
			}
		}
		else {
			neutralize(leftPos, leftEnd, rightPos, rightEnd);
			if(leftPos == leftEnd) {
				INT_ATOMIC_ADD(&threads_finished_left, 1);
			}
			if(rightPos == rightEnd) {
				INT_ATOMIC_ADD(&threads_finished_right, 1);
			}
		}
	}

	if(localId == 0) {
		// Calculate pivot position
		ptrdiff_t pp = leftStart + leftBlock * BLOCK_SIZE;
		if(leftPos > pp) {
			pp = leftPos;
			while(data[pp] < pivot) {
				++pp;
			}
		}else if(rightPos < pp) {
			pp = rightPos + 1;
			while(pp > 0 && data[pp - 1] >= pivot) {
				--pp;
			}
		}
		else {
			if(data[pp] < pivot)
				pp++;
			else if(pp > 0 && data[pp - 1] >= pivot) {
				--pp;
			}
		}



		assert(pp >= 0 && pp < (ptrdiff_t)length);
		assert(data[pp] >= pivot);
		assert(pp == 0 || data[pp-1] < pivot);

		if(pp < (((ptrdiff_t)length) - 1)) {
			swap(data[length - 1], data[pp]);
		}
		MEMORY_FENCE();
		pivotPosition = pp;
	//	cout << "pivot pos: " << pivotPosition << endl;
	//	cout << "Checking results" << endl;

	}
}

template <class Task, size_t BLOCK_SIZE>
void MixedModeQuicksortTask<Task, BLOCK_SIZE>::neutralize(ptrdiff_t &leftPos, ptrdiff_t &leftEnd, ptrdiff_t &rightPos, ptrdiff_t &rightEnd) {
	while(true) {
		while(leftPos < leftEnd) {
			if(data[leftPos] > pivot)
				break;
			leftPos++;
		}
		while(rightPos > rightEnd) {
			if(data[rightPos] < pivot)
				break;
			rightPos--;
		}
		if(leftPos == leftEnd || rightPos == rightEnd)
			break;
		assert(leftPos < rightPos && leftPos >= 0 && rightPos < (((ptrdiff_t)length) - 1));
		swap(data[leftPos], data[rightPos]);
	}
}

template <class Task, size_t BLOCK_SIZE>
bool MixedModeQuicksortTask<Task, BLOCK_SIZE>::is_partitioned() {
	for(size_t i = 0; i < length; i++) {
		if(i < pivotPosition && data[i] > pivot) {
			return false;
		//	cout << "data too large! " << i << endl;
		}
		else if(i > pivotPosition && data[i] < pivot) {
			return false;
		//	cout << "data too small! " << i << endl;
		}
		else if(i == pivotPosition && data[i] != pivot) {
			return false;
		/*	cout << "wrong pivot at " << data << " "<< (data + i) << ": " << i << " (" << (leftStart + leftBlock * BLOCK_SIZE) << ") " << pivot << " [" << data[i - 1] << "," << data[i] << "," << data[i+1] << "] " << data[length - 1] << " [" << leftPos << "," << rightPos << "] " << length << endl;
			cout << "tmp " << tmp << " tmp2 " << tmp2 << endl;
			cout << "right start " << rightStart << " length " << length << endl;
			for(int j = 0; j < length; j++) {
				if(data[j] == pivot) {
					cout << "pivot at " << j << endl;
				}
			}
			*/
		}
	}
	return true;
}

template <class Task, size_t BLOCK_SIZE>
void MixedModeQuicksortTask<Task, BLOCK_SIZE>::assert_is_partitioned() {
	for(size_t i = 0; i < length; i++) {
		assert(i >= pivotPosition || data[i] <= pivot);
		assert(i <= pivotPosition && data[i] >= pivot);
		assert(i != pivotPosition || data[i] == pivot);
	}
}

}

#endif /* MIXEDMODEQUICKSORTTASK_H_ */
