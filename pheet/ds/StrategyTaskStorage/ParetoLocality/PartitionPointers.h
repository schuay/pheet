#ifndef PARTITION_POINTERS_H_
#define PARTITION_POINTERS_H_

#include <vector>

class PartitionPointers
{
public:
	PartitionPointers(size_t block_size, size_t last_element = 0)
		: m_last(0), m_dead(block_size), m_end(last_element) {
		m_idx.push_back(0);
	}

	void fall_back() {
		m_dead = m_last;
		m_idx.pop_back();
		assert(m_idx.size() > 0);
		m_last = m_idx.back();
	}

	bool can_fall_back() {
		return m_idx.size() > 1;
	}


public: //methods required for white box testing
	size_t at(size_t idx) {
		return m_idx[idx];
	}

	size_t size() {
		return m_idx.size();
	}

	size_t last() {
		return m_last;
	}

	void increase_last() {
		m_last++;
	}

	void last(size_t val) {
		m_last = val;
	}

	//TODO: dead is not a good name, since it is also used for dead tasks
	size_t dead() {
		return m_dead;
	}

	void dead(size_t val) {
		m_dead = val;
	}

	void decrease_dead() {
		m_dead--;
	}

	size_t end() {
		return m_end;
	}

	void end(size_t val) {
		m_end = val;
	}

	void add(size_t idx) {
		m_idx.push_back(idx);
	}

private:
	std::vector<size_t> m_idx;
	/* start of last partition (excluding dead items) */
	size_t m_last;
	/* Dead items are stored right of the right-most partition, i.e., at the
	 * very end */
	size_t m_dead;
	/* end of block. Points to (last+1) item, i.e., last item is at end-1 */
	size_t m_end;

};

#endif /* PARTITION_POINTERS_H_ */
