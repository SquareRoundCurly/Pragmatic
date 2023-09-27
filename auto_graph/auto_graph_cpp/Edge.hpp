#pragma once

#include "Node.hpp"

// Forward decalartions
namespace SRC::auto_graph
{
	class Graph;
}

namespace SRC::auto_graph
{
	struct Edge
	{
		Node source;
		Node target;
		
		PythonTask task;

		bool operator==(const Edge &other) const
		{
			return source == other.source && target == other.target;
		}

		friend std::ostream& operator<<(std::ostream &os, const Edge &edge)
		{
			os << edge.source << " -> " << edge.target;
			return os;
		}
	};

	int Register_PyEdge(PyObject* module);
	PyObject* CreatePyEdge(std::string source, std::string target, SRC::auto_graph::Graph* graph);
} // namespace SRC::auto_graph