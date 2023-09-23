#pragma once

// Standard library
#include <string>
#include <ostream>

// Forward declarations
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
	struct Node
	{
		std::string id;

		Node(const std::string& name);

		bool operator==(const Node &other) const
		{
			return id == other.id;
		}

		friend std::ostream& operator<<(std::ostream &os, const Node &node)
		{
			os << node.id;
			return os;
		}
	};

	int Register_PyNode(PyObject* module);
	PyObject* CreatePyNode(const Node& node);
} // namespace SRC::auto_graph

namespace std
{
	template <>
	struct hash<SRC::auto_graph::Node>
	{
		size_t operator()(const SRC::auto_graph::Node &node) const
		{
			return hash<std::string>()(node.id);
		}
	};
}