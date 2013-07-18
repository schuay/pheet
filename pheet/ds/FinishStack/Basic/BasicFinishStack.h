/*
 * BasicFinishStack.h
 *
 *  Created on: Jul 17, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef BASICFINISHSTACK_H_
#define BASICFINISHSTACK_H_

#include <atomic>

namespace pheet {

struct BasicFinishStackElement {
	// Modified by local thread. Incremented when task is spawned, decremented when finished
	std::atomic<size_t> num_spawned;

	// Only modified by other threads. Stolen tasks that were finished (including spawned tasks)
	std::atomic<size_t> num_finished_remote;

	// Pointer to num_finished_remote of another thread (the one we stole tasks from)
	BasicSchedulerPlaceStackElement* parent;

	// Counter to update atomically when finalizing this element (ABA problem)
	std::atomic<size_t> version;
};

template <class Pheet, size_t StackSize>
class BasicFinishStackImpl {
public:
	typedef BasicFinishStackElement Element;

	BasicFinishStack()
	: stack_filled_left(0),
	  stack_filled_right(StackSize),
	  stack_init_left(0) {

	}

	~BasicFinishStack() {

	}

	Element* create_blocking(Element* parent) {

	}

	Element* create_non_blocking(Element* parent) {
//		performance_counters.num_non_blocking_finish_regions.incr();

		// Perform cleanup on finish stack
		empty_stack();

		pheet_assert(stack_filled_left < stack_filled_right);

		stack[stack_filled_left].num_spawned.store(1, std::memory_order_relaxed);
		stack[stack_filled_left].parent = parent;

		if(stack_filled_left >= stack_init_left) {
			stack[stack_filled_left].version.store(0, std::memory_order_relaxed);
			++stack_init_left;
		}
		else {
			size_t v = stack[stack_filled_left].version.load(std::memory_order_relaxed);
			stack[stack_filled_left].version.store(v + 1, std::memory_order_relaxed);
		}
		// As we create it out of a parent region that is waiting for completion of a single task, we can already add this one task here
		stack[stack_filled_left].num_finished_remote.store(0, std::memory_order_release);

		pheet_assert((stack[stack_filled_left].version & 1) == 0);

		++stack_filled_left;
//		performance_counters.finish_stack_nonblocking_max.add_value(stack_filled_left);

		return &(stack[stack_filled_left - 1]);
	}

	void spawn(Element* element) {
		size_t l = element->num_spawned.load(std::memory_order_relaxed);
		element->num_spawned.store(l + 1, std::memory_order_relaxed);
	}

	void signal_completion(Element* element) {
		// Happens before relationship between create_(non_)blocking/spawn and signal_completion
		// needs to be established externally

//		performance_counters.num_completion_signals.incr();
		Element* parent = element->parent;
		size_t version = element->version.load(std::memory_order_relaxed);

		bool local = element >= stack && (element < (stack + stack_size));
		size_t l;
		size_t r;
		// Ensure that the write always happens before the read.
		if(local) {
			pheet_assert(element->num_spawned.load(std::memory_order_relaxed) > r);
			l = element->num_spawned.load(std::memory_order_relaxed) - 1;
			element->num_spawned.store(l, std::memory_order_relaxed);

			// Establish happens-before between store and load
			std::atomic_thread_fence(std::memory_order_acq_rel);

			r = element->num_finished_remote.load(std::memory_order_relaxed);
		}
		else {
			r = element->num_finished_remote.fetch_add(1, std::memory_order_acq_rel) + 1;
			l = element->num_spawned.load(std::memory_order_relaxed);
		}
		if(l == r) {
			finalize(element, parent, version, local);
		}
	}

	bool unique(Element* parent);

private:
	void empty_stack() {
		while(stack_filled_left > 0) {
			size_t se = stack_filled_left - 1;
			if(stack[se].num_spawned.load(std::memory_order_relaxed) == stack[se].num_finished_remote.load(std::memory_order_relaxed)
					&& (stack[se].version.load(std::memory_order_relaxed) & 1)) {
				stack_filled_left = se;
			}
			else {
				break;
			}
		}
	}

	void finalize(Element* element, Element* parent, size_t version, bool local) {
		if(parent != NULL) {
			// We have to check if we are local too!
			// (otherwise the owner might already have modified element, and then num_spawned might be 0)
			// Rarely happens, but it happens!
			size_t version = element->version.load(std::memory_order_relaxed);
			if(local && element->num_spawned == 0) {
				pheet_assert(element >= stack && element < (stack + StackSize));
				// No tasks processed remotely - no need for atomic ops
			//	element->parent = NULL;
				element->version.store(version + 1, std::memory_order_relaxed);
//				performance_counters.num_chained_completion_signals.incr();
				signal_task_completion(parent);
			}
			else {
				if(element->version.compare_exchange_strong(version, version + 1, std::memory_order_release, std::memory_order_relaxed)) {
//					performance_counters.num_chained_completion_signals.incr();
//					performance_counters.num_remote_chained_completion_signals.incr();
					signal_completion(parent);
				}
			}
		}
	}

	Element[StackSize] stack;

	size_t stack_filled_left;
	size_t stack_filled_right;
	size_t stack_init_left;
};


} /* namespace pheet */
#endif /* BASICFINISHSTACK_H_ */
