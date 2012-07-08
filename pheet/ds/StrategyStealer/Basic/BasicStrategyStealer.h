/*
 * BasicStrategyStealer.h
 *
 *  Created on: Jun 22, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef BASICSTRATEGYSTEALER_H_
#define BASICSTRATEGYSTEALER_H_

#include <pheet/ds/StrategyHeap/Volatile2/VolatileStrategyHeap2.h>
#include "BasicStrategyStealerPerformanceCounters.h"

#include <unordered_map>

namespace pheet {

template <class Pheet, class TaskStorage>
class BasicStrategyStealerPlace {
public:
	typedef typename TaskStorage::Stream Stream;
	typedef typename Stream::StreamRef StreamRef;
	typedef typename StreamRef::StrategyRetriever StreamRefStrategyRetriever;
	typedef VolatileStrategyHeap2<Pheet, StreamRef, StreamRefStrategyRetriever> StrategyHeap;
	typedef typename StrategyHeap::PerformanceCounters StrategyHeapPerformanceCounters;
	typedef BasicStrategyStealerPerformanceCounters<Pheet, StrategyHeapPerformanceCounters> PerformanceCounters;

	typedef typename TaskStorage::T T;

	BasicStrategyStealerPlace(TaskStorage& task_storage, TaskStorage& target_task_storage, PerformanceCounters pc)
	:stream(task_storage), target_task_storage(target_task_storage), pc(pc), items(StreamRefStrategyRetriever(), pc.strategy_heap_performance_counters) {}

	T steal() {
		// First try to take a single item
		T ret;
		StreamRef top;
		size_t pre_weight;
		do {
			while(stream.has_next()) {
				pc.num_stream_tasks.incr();
				stream.next();
				stream.stealer_push_ref(*this);
			}
			if(items.empty()) {
				return nullable_traits<T>::null_value;
			}
			pre_weight = items.transitive_weight();
			top = items.pop();
		} while(!top.take(ret));

		pc.num_stolen_tasks.incr();

		// Now try to fill the own task storage with the rest
		// Has a separate linearization point, so a task with higher priority than ret may be added
		size_t weight = pre_weight - items.transitive_weight();
//		size_t total_stolen = 1;
		do {
			do {
				while(stream.has_next()) {
					pc.num_stream_tasks.incr();
					stream.next();
					stream.stealer_push_ref(*this);
				}
				if(items.empty()) {
					return ret;
				}
				pre_weight = items.transitive_weight();
				top = items.pop();
			} while(!stream.task_storage_push(target_task_storage, top));

			pc.num_stolen_tasks.incr();

//			++total_stolen;
			weight += pre_weight - items.transitive_weight();
		}while(weight < items.transitive_weight());

		return ret;
	}

	template <class Strategy>
	void push(StreamRef stream_ref) {
		items.template push<Strategy>(stream_ref);
	}

private:
	Stream stream;
	TaskStorage& target_task_storage;
	PerformanceCounters pc;
	StrategyHeap items;
};

template <class Pheet, class TaskStorage>
class BasicStrategyStealer {
public:
	typedef BasicStrategyStealerPlace<Pheet, TaskStorage> StealerPlace;
	typedef StealerPlace StealerRef;
	typedef VolatileStrategyHeap2PerformanceCounters<Pheet> StrategyHeapPerformanceCounters;
	typedef BasicStrategyStealerPerformanceCounters<Pheet, StrategyHeapPerformanceCounters> PerformanceCounters;

	BasicStrategyStealer(TaskStorage& local_task_storage, PerformanceCounters& pc)
	:local_task_storage(local_task_storage), pc(pc) {}
	~BasicStrategyStealer() {
		for(auto i = places.begin(); i != places.end(); ++i) {
			delete (i->second);
		}
	}

	typename TaskStorage::T steal_from(typename Pheet::Scheduler::Place* place) {
		if(Pheet::get_place_id() < place->get_id()) {
			return nullable_traits<typename TaskStorage::T>::null_value;
		}

		StealerPlace*& p = places[place];
		if(p == nullptr) {
			p = new StealerPlace(place->get_task_storage(), local_task_storage, pc);
		}
		return p->steal();
	//	return nullable_traits<typename TaskStorage::T>::null_value;
	}

private:
	TaskStorage& local_task_storage;
	std::unordered_map<typename Pheet::Scheduler::Place*, StealerPlace*> places;

	PerformanceCounters pc;
};

} /* namespace pheet */
#endif /* BASICSTRATEGYSTEALER_H_ */
