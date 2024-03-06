#pragma once

// Standard library
#include <string>

// Python
#include "Python.h"
#include "pytypedefs.h"

namespace Pragmatic::auto_graph
{
	enum class Owner
	{
		CPP,
		PY
	};

	struct Node
	{
		std::string name;
		PyObject* task;
		int generation = 0;
		// Other data fields specific to the node

		Node(const std::string& n, PyObject* task = nullptr);
	};

	struct PyNode
	{
		PyObject_HEAD;
		Owner owner;
		Node* node;
	};

	PyTypeObject* GetPyTypeObject();
} // namespace Pragmatic::auto_graph