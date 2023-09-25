#pragma once

// Standard library
#include <string>
#include <ostream>

// auto_graph
#include "python_interpreter.hpp"


// Forward declarations
struct _object;
typedef struct _object PyObject;
namespace SRC::auto_graph
{
	class Graph;
}

namespace SRC::auto_graph
{
	struct Node
	{
		std::string name;
		PythonTask task;

		Node(const std::string& name);
		Node(const std::string& name, PythonTask task);

		bool operator==(const Node &other) const
		{
			return name == other.name;
		}

		friend std::ostream& operator<<(std::ostream &os, const Node &node)
		{
			os << node.name;
			return os;
		}
	};

	int Register_PyNode(PyObject* module);
	PyObject* CreatePyNode(std::string name, SRC::auto_graph::Graph* graph);
	Node GetNode(PyObject* pyNode);
} // namespace SRC::auto_graph

namespace std
{
	template <>
	struct hash<SRC::auto_graph::Node>
	{
		size_t operator()(const SRC::auto_graph::Node &node) const
		{
			return hash<std::string>()(node.name);
		}
	};
}