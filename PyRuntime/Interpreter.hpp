#pragma once

// External
#include "pytypedefs.h"

namespace Pragmatic::auto_graph
{
	class Interpreter
	{
		public:
		virtual PyObject* Execute(PyObject* callable, PyObject* args, PyObject* kwArgs) = 0;
	};
	
} // namespace Pragmatic::auto_graph