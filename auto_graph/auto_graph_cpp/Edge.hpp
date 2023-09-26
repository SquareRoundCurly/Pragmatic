#pragma once

#include "Node.hpp"
#include "Graph.hpp"

namespace SRC::auto_graph
{
	struct Edge
	{
		Node from;
		Node to;
		
		PythonTask task;
	};
} // namespace SRC::auto_graph
