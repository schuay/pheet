/*
 * KLSMLocalityTaskStorageFrame.h
 *
 *  Created on: Sep 25, 2013
 *      Author: Martin Wimmer
 *	   License: Boost Software License 1.0
 */

#ifndef KLSMLOCALITYTASKSTORAGEFRAME_H_
#define KLSMLOCALITYTASKSTORAGEFRAME_H_

#include <limits>

namespace pheet {

template <class Pheet>
class KLSMLocalityTaskStorageFrame {
public:
	KLSMLocalityTaskStorageFrame()
	:phase(0) {
		registered[0].store(0, std::memory_order_relaxed);
		registered[1].store(-1, std::memory_order_relaxed);
	}
	~KLSMLocalityTaskStorageFrame() {}

	bool can_progress_phase() {
		size_t p = phase.load(std::memory_order_relaxed);

		// Check status of previous phase
		s_procs_t ro = registered[(p&1)^1].load(std::memory_order_relaxed);
		if(ro == -1) {
			// Previous phase has already been finished
			return true;
		}
		else if(ro == 0) {
			// We can try to finalize the previous phase
			if(registered[(p&1)^1].compare_exchange_strong(ro, -1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
				// Previous phase is finalized now
				return true;
			}
		}
		return false;
	}

	void progress_phase() {
		size_t p = phase.load(std::memory_order_relaxed);
		pheet_assert(registered[(p&1)^1].load(std::memory_order_relaxed) == -1);

		// First reopen the position for the new phase
		registered[(p&1)^1].store(0, std::memory_order_release);
		// Then release new phase id (reopening of position needs to have happened before)
		phase.store((p+1) & wraparound, std::memory_order_release);
	}

	bool can_reuse(ptrdiff_t last_phase) {
		pheet_assert(last_phase >= 0);

		size_t p = phase.load(std::memory_order_relaxed);
		if((size_t)last_phase == ((p+1) & wraparound) ||
				(size_t)last_phase == p) {
			// If last_phase of item is bigger than the current phase, this means that
			// in practice it was already freed one phase earlier. So no need to
			// progress two phases

			if(can_progress_phase()) {
				progress_phase();

				if(can_progress_phase()) {
					return true;
				}
			}
			return false;
		}

		return true;
	}

	size_t register_place() {
		while(true) {
			size_t p = phase.load(std::memory_order_relaxed);
			s_procs_t r = registered[p&1].load(std::memory_order_acquire);

			pheet_assert(r >= -1);

			while(r == -1 || !registered[p&1].compare_exchange_weak(r, r + 1, std::memory_order_acquire, std::memory_order_acquire)) {
				p = phase.load(std::memory_order_relaxed);
				r = registered[p&1].load(std::memory_order_acquire);
			}

			// Check if the phase is still the same
			size_t p2 = phase.load(std::memory_order_acquire);
			if((p2 & 1) == (p & 1)) {
				// As long as the new phase uses the same slot we are fine
				return p2;
			}
			deregister_place(p);
		}
	}

	/*
	 * Wait-free variant
	 */
	bool try_register_place(size_t& ret_phase) {
		size_t p = phase.load(std::memory_order_relaxed);
		s_procs_t r = registered[p&1].load(std::memory_order_acquire);

		pheet_assert(r >= -1);

		if(r == -1 || !registered[p&1].compare_exchange_weak(r, r + 1, std::memory_order_acquire, std::memory_order_acquire)) {
			return false;
		}
		size_t p2 = phase.load(std::memory_order_acquire);
		if((p2 & 1) == (p & 1)) {
			ret_phase = p2;
			return true;
		}
		deregister_place(p);
		return false;
	}

	void deregister_place(size_t phase) {
		registered[phase&1].fetch_sub(1, std::memory_order_acq_rel);
	}

	/*
	 * Uppermost bit is never used, so it is safe to be casted to a signed type as well
	 */
	size_t get_phase() {
		return phase.load(std::memory_order_relaxed);
	}

	size_t get_take_phase() {
		return (phase.load(std::memory_order_relaxed) + 1) & wraparound;
	}

	/*
	 * Frames with medium contention can be used for more items if already in use
	 */
	bool medium_contention() const {
		size_t pi = phase.load(std::memory_order_relaxed) & 1;
		return registered[pi^1].load(std::memory_order_relaxed) == -1;
	}

	/*
	 * Frames with low contention can be reused for new items
	 */
	bool low_contention() const {
		size_t pi = phase.load(std::memory_order_relaxed) & 1;
		return registered[pi].load(std::memory_order_relaxed) == 0 &&
				registered[pi^1].load(std::memory_order_relaxed) == -1;
	}

private:
	std::atomic<s_procs_t> registered[2];
	std::atomic<size_t> phase;

	static size_t wraparound;
};

template <class Pheet, class Frame>
class KLSMLocalityTaskStorageFrameRegistration {
public:
	KLSMLocalityTaskStorageFrameRegistration()
	:references(0), phase(0) {

	}

	void add_ref(Frame* frame) {
		if(references == 0) {
			phase = frame->register_place();
		}
	/*	else if(frame->get_phase() != phase) {
			size_t new_phase = frame->register_place();
			frame->deregister_place(phase);
			phase = new_phase;
		}*/
		++references;
	}

	bool try_add_ref(Frame* frame) {
		if(references == 0) {
			if(!frame->try_register_place(phase)) {
				return false;
			}
		}
		++references;
		return true;
	}

	void rem_ref(Frame* frame) {
		pheet_assert(references > 0);
		--references;
		if(references == 0) {
			frame->deregister_place(phase);
		}
	/*	else if(frame->get_phase() != phase) {
			size_t new_phase = frame->register_place();
			frame->deregister_place(phase);
			phase = new_phase;
		}*/
	}

	size_t get_phase() {
		return phase;
	}
private:
	size_t references;
	size_t phase;
};

/*
 * Making the wraparound below the highest bit simplifies wraparound and allows for safe casting to
 * signed types
 */
template <class Pheet>
size_t KLSMLocalityTaskStorageFrame<Pheet>::wraparound = std::numeric_limits<size_t>::max() >> 1;


template <class Frame>
struct KLSMLocalityTaskStorageFrameReuseCheck {
	bool operator() (Frame const& frame) const {
		return frame.low_contention();
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEFRAME_H_ */
