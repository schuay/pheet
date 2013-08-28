#ifndef __SHORTEST_PATHS_H
#define __SHORTEST_PATHS_H

#include <unordered_map>

#include "test/msp/lib/Graph/Node.h"
#include "Path.h"

namespace sp
{

class ShortestPaths
{
public:
	void print() const;

public:
	std::unordered_map<const graph::Node*, Paths> paths;
};

}

#endif /* __SHORTEST_PATHS_H */
