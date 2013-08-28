#include "ShortestPaths.h"

#include <stdio.h>

namespace sp
{

void
ShortestPaths::
print() const
{
	for (const auto & p : paths) {
		for (const auto & path : p.second) {
			path->print();
		}
		printf("\n");
	}
}

}
