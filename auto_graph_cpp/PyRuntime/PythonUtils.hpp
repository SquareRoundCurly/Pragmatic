#pragma once

// Standard library
#include <string>

// External
#include "pytypedefs.h"

namespace Pragmatic::auto_graph
{
	PyObject* ensure_tuple(PyObject* obj);
	PyObject* merge_tuples(PyObject* args1, PyObject* args2);

	void ThrowPythonError(const std::string& message);
} // namespace Pragmatic::auto_graph
