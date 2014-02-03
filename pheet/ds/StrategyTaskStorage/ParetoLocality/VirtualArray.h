#ifndef VIRTUAL_ARRAY_H_
#define VIRTUAL_ARRAY_H_

#include "VirtualArrayBlock.h"

#define DATA_BLOCK_SIZE (2)


/**
 * A virtual array of infinite size
 */
template <class T>
class VirtualArray
{
public:
	VirtualArray()
		: m_block_cnt(1) {
		m_first = new Block();
		m_last = m_first;
	}

	~VirtualArray() {
		while (m_first->next()) {
			m_first = m_first->next();
			delete m_first->prev();
		}
		delete m_first;
	}

	void push(T item) {
		if (m_last->size() == m_last->capacity()) {
			add_block();
		}
		m_last->push(item);
	}

	//TODO: implement element access using iterators instead?
	T& operator[](size_t idx) {
		assert(idx < size());

		Block* block =  find_block(idx);
		return (*block)[idx % block_size()];
	}

	void swap(size_t left, size_t right) {
		//TODO: tmp should be the last element of a block (so that spy finds the
		//element currently swapped out)
		T tmp = get(left);
		set(left, get(right));
		set(right, tmp);
	}

	size_t size() {
		return DATA_BLOCK_SIZE * (m_block_cnt - 1) + m_last->size();
	}

	size_t block_size() {
		return DATA_BLOCK_SIZE;
	}

private:
	typedef VirtualArrayBlock<T, DATA_BLOCK_SIZE> Block;

private:
	Block* find_block(size_t idx) {
		//find block that stores element at location idx
		Block* tmp = m_first;
		size_t cnt = block_size();
		//TODO: reduce asymptotic complexity
		while (cnt <= idx) {
			tmp = tmp->next();
			cnt += block_size();
		}
		return tmp;
	}

	void set(size_t idx, T& item) {
		Block* block  = find_block(idx);
		(*block)[idx % block_size()] = item;
	}

	T& get(size_t idx) {
		Block* block  = find_block(idx);
		return (*block)[idx % block_size()];
	}

	void add_block() {
		Block* tmp = new Block();
		tmp->prev(m_last);
		m_last->next(tmp);
		m_last = tmp;
		++m_block_cnt;
	}

private:
	Block* m_first;
	Block* m_last;
	size_t m_block_cnt;

};





#endif /* VIRTUAL_ARRAY_H_ */
