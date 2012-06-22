/*
 * BasicStrategyStealer.h
 *
 *  Created on: Jun 22, 2012
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef BASICSTRATEGYSTEALER_H_
#define BASICSTRATEGYSTEALER_H_

#include <pheet/ds/StrategyHeap/Volatile/VolatileStrategyHeap.h>

#include <unordered_map>

namespace pheet {

struct BasicStrategyStealerPerformanceCounters {
	static void print_headers() {}
	void print_values() {}
};

template <class Pheet, class TaskStorage>
class BasicStrategyStealerPlace {
public:
	typedef typename TaskStorage::Stream Stream;
	typedef typename Stream::StreamRef StreamRef;
	typedef typename StreamRef::StrategyRetriever StreamRefStrategyRetriever;
	typedef VolatileStrategyHeap<Pheet, StreamRef, StreamRefStrategyRetriever> StrategyHeap;

	typedef typename TaskStorage::T T;

	BasicStrategyStealerPlace(TaskStorage& task_storage)
	:stream(task_storage), items(StreamRefStrategyRetriever(), shpc) {}

	T steal(/*TaskStorage& target*/) {
		T ret;
		while(stream.has_next()) {
			stream.next();
			StreamRef ref = stream.get_ref();
			if(ref.take(ret)) {
				return ret;
			}
		}
		return nullable_traits<T>::null_value;
	/*	while(stream.has_next()) {
			stream.next();
			items.push(stream.get_ref());
		}
		if(items.empty())
			return nullable_traits<T>::null_value;
		StreamRef top = items.pop();
		while(!top.take(ret)) {
			if(items.empty()) {
				return nullable_traits<T>::null_value;
			}
			top = items.pop();
		}
		return ret;*/
	}

private:
	Stream stream;
	typename StrategyHeap::PerformanceCounters shpc;
	StrategyHeap items;
};

template <class Pheet, class TaskStorage>
class BasicStrategyStealer {
public:
	typedef BasicStrategyStealerPerformanceCounters PerformanceCounters;
	typedef BasicStrategyStealerPlace<Pheet, TaskStorage> StealerPlace;

	BasicStrategyStealer(/*TaskStorage& local_task_storage,*/ PerformanceCounters& pc)
	/*:local_task_storage(local_task_storage)*/ {}
	~BasicStrategyStealer() {
		for(auto i = places.begin(); i != places.end(); ++i) {
			delete (i->second);
		}
	}

	typename TaskStorage::T steal_from(typename Pheet::Scheduler::Place* place) {
		StealerPlace*& p = places[place];
		if(p == nullptr) {
			p = new StealerPlace(place->get_task_storage());
		}
		return p->steal();
	//	return nullable_traits<typename TaskStorage::T>::null_value;
	}

private:
//	TaskStorage& local_task_storage;
	unordered_map<typename Pheet::Scheduler::Place*, StealerPlace*> places;
};

} /* namespace pheet */
#endif /* BASICSTRATEGYSTEALER_H_ */
