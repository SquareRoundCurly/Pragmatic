#pragma once

// auto_graph
#include "Interpreter.hpp"

namespace Pragmatic::auto_graph
{
	class ProcessInterpreter : public Interpreter
	{
		public:
		ProcessInterpreter();
		~ProcessInterpreter();

		public:
		virtual PyObject* Execute(PyObject* callable, PyObject* args, PyObject* kwArgs);
	};
} // namespace Pragmatic::auto_graph