/*
 * TriStripResult.h
 *
 *  Created on: Jul 19, 2012
 *      Author: cederman
 */

#ifndef TRISTRIPRESULT_H_
#define TRISTRIPRESULT_H_

#include <pheet/pheet.h>


namespace pheet
{

template <class Pheet>
class TriStripResult
{
	SumReducer<Pheet, size_t, SumOperation> tristripcount;
	SumReducer<Pheet, size_t> nodecount;

public:
	TriStripResult() {};
	TriStripResult(TriStripResult<Pheet>& other):tristripcount(other.tristripcount),nodecount(other.nodecount) {}
	TriStripResult(TriStripResult<Pheet>&& other):tristripcount(other.tristripcount),nodecount(other.nodecount) {};

	void addstrip(std::vector<GraphNode*> strip)
	{
		nodecount.add(strip.size());
		// Should store the strip, but for now only count it
		tristripcount.incr();
	}

	size_t getNodeCount()
	{
		return nodecount.get_sum();
	}

	size_t getCount()
	{
		return tristripcount.get_sum();
	}
};


}

#endif /* TRISTRIPRESULT_H_ */
