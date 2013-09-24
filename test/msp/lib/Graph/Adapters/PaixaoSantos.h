/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PAIXAO_SANTOS_H
#define __PAIXAO_SANTOS_H

#include <istream>

#include "lib/Graph/Graph.h"

namespace graph
{

class PaixaoSantos
{
public:
	graph::Graph* operator()(std::istream& in) const;
};

}

#endif /* __PAIXAO_SANTOS_H */
