/*
 * BasicNextVertex.cpp
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "BasicNextVertex.h"

namespace pheet {

BasicNextVertex::BasicNextVertex() {

}

BasicNextVertex::~BasicNextVertex() {

}

size_t BasicNextVertex::operator()(GraphVertex* graph, size_t size, size_t k, std::set<size_t> const& set1, std::set<size_t> const& set2) {
	return set1.size() + set2.size();
}

char const BasicNextVertex::name[] = "BasicNextVertex";

}
