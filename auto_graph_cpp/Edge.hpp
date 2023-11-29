#pragma once

// auto_graph
#include "Node.hpp"

namespace Pragmatic::auto_graph
{
	struct Edge
	{
		Node* from;
		Node* to;
		// Other data fields specific to the edge

		Edge(Node* src, Node* dest) : from(src), to(dest) { }
	};
} // namespace Pragmatic::auto_graph