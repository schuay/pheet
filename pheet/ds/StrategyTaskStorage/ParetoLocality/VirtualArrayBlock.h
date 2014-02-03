#ifndef DATA_BLOCK_H_
#define DATA_BLOCK_H_

#include <assert.h>

template <class T, size_t N>
class VirtualArrayBlock
{
private:
	typedef VirtualArrayBlock<T, N> Self;

public:
	VirtualArrayBlock<T, N>()
		: m_size(0), m_next(nullptr), m_prev(nullptr) {

		for (size_t i = 0; i < N; i++) {
			data[i] = nullptr;
		}
	}

	void push(T item) {
		assert(m_size < N);
		data[m_size++] = item;
	}

	T pop() {
		assert(m_size > 0);
		return data[--m_size];
	}

	T& operator[](size_t idx) {
		assert(idx < N);
		return data[idx];
	}

	size_t size() {
		return m_size;
	}

	size_t capacity() {
		return N;
	}

	void next(Self* next) {
		m_next = next;
	}

	Self* next() {
		return m_next;
	}

	void prev(Self* prev) {
		m_prev = prev;
	}

	Self* prev() {
		return m_prev;
	}

private:
	T data[N];
	size_t m_size;
	Self* m_next;
	Self* m_prev;

};



#endif /* DATA_BLOCK_H_ */
