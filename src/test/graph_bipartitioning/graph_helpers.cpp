/*
 * graph_helpers.cpp
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "graph_helpers.h"

namespace pheet {

GraphBipartitioningSolution::GraphBipartitioningSolution() {
	weight = 0;
}

GraphBipartitioningSolution& GraphBipartitioningSolution::operator=(GraphBipartitioningSolution const& other) {
	weight = other.weight;
	return *this;
}

GraphBipartitioningSolution& GraphBipartitioningSolution::operator=(GraphBipartitioningSolution& other) {
	weight = other.weight;
	return *this;
}

GraphBipartitioningSolution& MaxOperation<GraphBipartitioningSolution>::operator()(GraphBipartitioningSolution& x, GraphBipartitioningSolution& y) {
	if(x.weight <= y.weight)
		return x;
	return y;
}

GraphBipartitioningSolution MaxOperation<GraphBipartitioningSolution>::get_identity() {
	GraphBipartitioningSolution ret;
	ret.weight = std::numeric_limits<size_t>::max();
	return ret;
}

}
