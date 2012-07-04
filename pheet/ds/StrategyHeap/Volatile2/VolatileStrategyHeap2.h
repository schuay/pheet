/*
 * VolatileStrategyHeap2.h
 *
 *  Created on: Jul 4, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef VOLATILESTRATEGYHEAP2_H_
#define VOLATILESTRATEGYHEAP2_H_

namespace pheet {

template <class Pheet, typename TT, class StrategyRetriever>
class VolatileStrategyHeap2 {
public:
	typedef TT T;
	typedef VolatileStrategyHeapBase<Pheet, T> HeapBase;
	typedef typename HeapBase::Node HeapNode;
	typedef typename Pheet::Scheduler::BaseStrategy BaseStrategy;
	typedef VolatileStrategyHeapPerformanceCounters<Pheet> PerformanceCounters;

	VolatileStrategyHeap2(StrategyRetriever sr, PerformanceCounters& pc);
	~VolatileStrategyHeap2();

	template <class Strategy>
	void push(T const& item);

	T pop();
	T& peek();

	bool empty() {
		return is_empty();
	}
	bool is_empty();
	size_t size() { return total_size; }
	size_t transitive_weight() { return total_weight; }

	static void print_name();

private:
	std::map<std::type_index, HeapBase*> heap_heaps;
	StrategyRetriever sr;
	size_t total_weight;
	VolatileStrategyHeapHeap<Pheet, TT, BaseStrategy, BaseStrategy, StrategyRetriever> root_heap;
	PerformanceCounters pc;

	size_t total_size;

};

} /* namespace pheet */
#endif /* VOLATILESTRATEGYHEAP2_H_ */
