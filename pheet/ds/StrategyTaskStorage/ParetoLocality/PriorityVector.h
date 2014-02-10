/*
 * Copyright Jakob Gruber, Martin Kalany 2014.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */


#ifndef WEIGHT_V_H_
#define WEIGHT_V_H_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "pheet/misc/assert.h"
#include "test/msp/lib/ShortestPath/Path.h"

template <size_t N>
class PriorityVector
{

public:

	PriorityVector() {}

	PriorityVector(std::vector<size_t>* w)
	{
		pheet_assert(w->size() == N);
		m_w = w;
	}

	//TODO: this is just a temp quick and dirty fix
	PriorityVector(sp::PathPtr path)
	{
		pheet_assert(path->degree() == N);
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
		pheet_assert(idx < N);
		return (*m_w)[idx];
	}

private:
	std::vector<size_t>* m_w;
};

#endif /* WEIGHT_V_H_ */
