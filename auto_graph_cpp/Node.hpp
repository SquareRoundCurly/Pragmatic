#pragma once

// Standard library
#include <string>

// Python
#include "pytypedefs.h"

namespace Pragmatic::auto_graph
{
	struct Node
	{
		std::string name;
		PyObject* task;
		int generation = 0;
		// Other data fields specific to the node

		Node(const std::string& n, PyObject* task = nullptr) : name(n), task(task) { }
	};
} // namespace Pragmatic::auto_graph