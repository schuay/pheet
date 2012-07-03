/*
 * RecursiveParallelPrefixSum.h
 *
 *  Created on: Jun 28, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef RECURSIVEPARALLELPREFIXSUM_H_
#define RECURSIVEPARALLELPREFIXSUM_H_

#include <iostream>
#include <pheet/pheet.h>
#include "RecursiveParallelPrefixSumOffsetTask.h"

namespace pheet {

template <class Pheet, size_t BlockSize>
class RecursiveParallelPrefixSumImpl : public Pheet::Task {
public:
	typedef RecursiveParallelPrefixSumImpl<Pheet, BlockSize> Self;
	typedef RecursiveParallelPrefixSumOffsetTask<Pheet, BlockSize> OffsetTask;

	RecursiveParallelPrefixSumImpl(unsigned int* data, size_t length)
	:data(data), length(length), step(1), root(true) {

	}

	RecursiveParallelPrefixSumImpl(unsigned int* data, size_t length, size_t step, bool root)
	:data(data), length(length), step(step), root(root) {

	}

	virtual ~RecursiveParallelPrefixSumImpl() {}

	virtual void operator()() {
		if(length <= BlockSize) {
			for(size_t i = 1; i < length; ++i) {
				data[i*step] += data[(i - 1)*step];
			}
		}
		else {
			size_t num_blocks = ((length - 1) / BlockSize) + 1;
			size_t half = BlockSize * (num_blocks >> 1);
			if(root)
			{
				{typename Pheet::Finish f;
					Pheet::template
						spawn<Self>(data, half, step, false);
					Pheet::template
						spawn<Self>(data + half*step, length - half, step, false);
				}
//std::cout << data[1024] << std::endl;
				if(num_blocks > 2)
					Pheet::template
						finish<Self>(data + (BlockSize - 1)*step, num_blocks - 1, BlockSize * step, true);
//				std::cout << data[1024] << std::endl;

				Pheet::template
					call<OffsetTask>(data + (BlockSize*step), length - BlockSize, step);
			}
			else {
				Pheet::template
					spawn<Self>(data, half, step, false);
				Pheet::template
					spawn<Self>(data + half*step, length - half, step, false);
			}
		}
	}

	static char const name[];

private:
	unsigned int* data;
	size_t length;
	size_t step;
	bool root;
};

template <class Pheet, size_t BlockSize>
char const RecursiveParallelPrefixSumImpl<Pheet, BlockSize>::name[] = "RecursiveParallelPrefixSum";

template <class Pheet>
using RecursiveParallelPrefixSum = RecursiveParallelPrefixSumImpl<Pheet, 1024>;

} /* namespace pheet */
#endif /* RECURSIVEPARALLELPREFIXSUM_H_ */
