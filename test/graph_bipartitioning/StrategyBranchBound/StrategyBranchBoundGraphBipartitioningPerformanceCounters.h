/*
 * StrategyBranchBoundGraphBipartitioningPerformanceCounters.h
 *
 *  Created on: Feb 15, 2012
 *      Author: mwimmer
 *	   License: Ask Author
 */

#ifndef STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_
#define STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_

namespace pheet {

class StrategyBranchBoundGraphBipartitioningPerformanceCounters {
public:
	typedef StrategyBranchBoundGraphBipartitioningPerformanceCounters<Pheet> Self;

	StrategyBranchBoundGraphBipartitioningPerformanceCounters() {}
	StrategyBranchBoundGraphBipartitioningPerformanceCounters(Self& other) {}
	~StrategyBranchBoundGraphBipartitioningPerformanceCounters() {}

	static void print_headers() {}
	void print_values() {}
};

}

#endif /* STRATEGYBRANCHBOUNDGRAPHBIPARTITIONINGPERFORMANCECOUNTERS_H_ */
