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
		PyObject* task;

		Edge(Node* src, Node* dest, PyObject* task) : from(src), to(dest), task(task) { }
	};
} // namespace Pragmatic::auto_graph