/*
 * AutoStrategy.h
 *
 *  Created on: Oct 10, 2011
 *      Author: mwimmer
 */

#ifndef AUTOSTRATEGY_H_
#define AUTOSTRATEGY_H_

#include <iostream>

namespace pheet {

template <class Scheduler, template <class EScheduler> class Strategy>
class BranchBoundGraphBipartitioningAutoStrategy {
public:
	BranchBoundGraphBipartitioningAutoStrategy();
	~BranchBoundGraphBipartitioningAutoStrategy();

	Strategy<Scheduler> operator()(GraphVertex* graph, size_t size, size_t k, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub, size_t lb);

	static void print_name();
};

template <class Scheduler, template <class EScheduler> class Strategy>
inline BranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy>::BranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Scheduler, template <class EScheduler> class Strategy>
inline BranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy>::~BranchBoundGraphBipartitioningAutoStrategy() {

}

template <class Scheduler, template <class EScheduler> class Strategy>
inline Strategy<Scheduler> BranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy>::operator()(GraphVertex* graph, size_t size, size_t k, size_t* set1, size_t set1_size, size_t* set2, size_t set2_size, size_t* ub, size_t lb) {
	return Strategy<Scheduler>();
}

template <class Scheduler, template <class EScheduler> class Strategy>
void BranchBoundGraphBipartitioningAutoStrategy<Scheduler, Strategy>::print_name() {
	std::cout << "AutoStrategy<";
	Strategy<Scheduler>::print_name();
	std::cout << ">";
}

}

#endif /* AUTOSTRATEGY_H_ */
