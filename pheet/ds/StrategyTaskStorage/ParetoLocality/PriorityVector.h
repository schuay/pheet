#ifndef WEIGHT_V_H_
#define WEIGHT_V_H_

#include <assert.h>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "test/msp/lib/ShortestPath/Path.h"

template <size_t N>
class PriorityVector
{

public:

	PriorityVector() {}

	PriorityVector(std::vector<size_t>* w)
	{
		assert(w->size() == N);
		m_w = w;
	}

	//TODO: this is just a temp quick and dirty fix
	PriorityVector(sp::PathPtr path)
	{
		assert(path->degree() == N);
		m_w = new std::vector<size_t>();
		for (size_t i = 0; i < N; i++) {
			m_w->push_back(path->weight()[i]);
		}

	}

	size_t dimensions() const
	{
		return N;
	}

	void print() const
	{
		std::cout << "[";
		for (size_t i = 0; i < N; i++) {
			std::cout << (*m_w)[i];
			if (i < N - 1) {
				std::cout << ", ";
			}
		}
		std::cout << "]";
	}

	size_t at(size_t idx) const
	{
		assert(idx < N);
		return (*m_w)[idx];
	}

private:
	std::vector<size_t>* m_w;
};

#endif /* WEIGHT_V_H_ */
