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

	bool can_reuse(procs_t last_phase) {
		procs_t p = phase.load(std::memory_order_relaxed);
		if(last_phase == ((p+1) & wraparound)) {
			// Item was freed in this phase. We need to progress phases to be able to free it
			phase.store((p+1) & wraparound, std::memory_order_relaxed);

			// Now let us try and finalize old phase by closing previous phase
			signed procs_t r = registered[p&1].load(std::memory_order_relaxed);
			if(r == 0) {
				if(registered[p&1].compare_exchange_strong(r, -1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
					// Now check whether there are still threads registered
					// Since cas does an acquire all changes to this field before are also covered
					// (they are released using a fetch_and_sub to the CAS'd field)
					signed procs_t ro = registered[(p&1)^1].load(std::memory_order_relaxed);

					if(ro == 0) {
						// No threads in phase, reuse is safe
						return true;
					}
				}
			}
			return false;
		}

		if(last_phase == p) {
			signed procs_t ro = registered[(p&1)^1].load(std::memory_order_relaxed);
			if(ro == -1) {
				// Last phase has been finished

				// Check whether no thread is in current phase
				if(registered[p&1].load(std::memory_order_relaxed) == 0) {
					// Update to last_phase happened before next thread will register for frame
					// It is safe to reuse the item
					return true;
				}
				else {
					// Need to progress to next phase for a chance of progress
					// First reopen the position for the new phase
					registered[(p&1)^1].store(0, std::memory_order_relaxed);
					// Then release new phase id (reopening of position needs to have happened before)
					phase.store((p+1) & wraparound, std::memory_order_release);

					// No need to try and close old position, since we just found out it is != 0
					return false;
				}
			}
			else if(ro == 0) {
				// We still need to close the old position with a CAS

				if(registered[(p&1)^1].compare_exchange_strong(ro, -1, std::memory_order_acq_rel, std::memory_order_relaxed)) {
					// Check whether no thread is in current phase
					// Since cas does an acquire all changes to this field before are also covered
					// (they are released using a fetch_and_sub to the CAS'd field)
					if(registered[p&1].load(std::memory_order_relaxed) == 0) {
						// Update to last_phase happened before next thread will register for frame
						// It is safe to reuse the item
						return true;
					}
				}
				return false
			}
			// Old phase is still in use, nothing we can do
			return false;
		}

		// All other phases can be reused
		return true;
	}

	/*
	 * Uppermost bit is never used, so it is safe to be casted to a signed type as well
	 */
	size_t get_phase() {
		return phase.load(std::memory_order_relaxed);
	}

	/*
	 * Frames with low contention can be reused for new items
	 */
	bool low_contention() {
		size_t ri = phase.load(std::memory_order_relaxed) & 1;
		return registered[ri].load(std::memory_order_relaxed) == 0 &&
				registered[ri^1].load(std::memory_order_relaxed) == -1;
	}

private:
	std::atomic<signed procs_t> registered[2];
	std::atomic<size_t> phase;

	static size_t wraparound;
};

/*
 * Making the wraparound below the highest bit simplifies wraparound and allows for safe casting to
 * signed types
 */
template <class Pheet>
size_t KLSMLocalityTaskStorageFrame<Pheet>::wraparound = std::numeric_limits<procs_t>::max() >> 1;


template <class Frame>
struct KLSMLocalityTaskStorageFrameReuseCheck {
	bool operator() (Frame const& frame) const {
		return frame.low_contention();
	}
};

} /* namespace pheet */
#endif /* KLSMLOCALITYTASKSTORAGEFRAME_H_ */
