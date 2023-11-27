#pragma once

// auto_graph
#include "Node.hpp"

namespace Pragmatic::auto_graph
{
	struct Edge 
	{
		int data;
		Node* source;
		Node* destination;

		// Constructor
		Edge(Node* src, Node* dest) : source(src), destination(dest) { }
	};
} // namespace Pragmatic::auto_graph