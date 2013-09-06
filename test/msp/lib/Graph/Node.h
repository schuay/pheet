/*
 * Copyright Jakob Gruber, Martin Kalany 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __NODE_H
#define __NODE_H

#include <vector>

namespace graph
{

class Node
{
public:
    uint id() const;
    Graph const* graph() const;

    std::vector<Edge> const& edges() const;

private:
    Node(Graph* graph,
         const uint id);

private:
    Graph const* m_graph;
    const uint m_id;

    std::vector<Edge> m_edges;
};

}

#endif /* __NODE_H */
