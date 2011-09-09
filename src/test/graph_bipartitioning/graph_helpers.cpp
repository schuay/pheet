/*
 * graph_helpers.cpp
 *
 *  Created on: 09.09.2011
 *   Author(s): Martin Wimmer
 *     License: Ask author
 */

#include "graph_helpers.h"

namespace pheet {

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
