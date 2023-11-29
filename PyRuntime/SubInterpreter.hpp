#pragma once

// auto_graph
#include "Interpreter.hpp"

namespace Pragmatic::auto_graph
{
	class SubInterpreter : public Interpreter
	{
		public: // Ctor / Dtor
		SubInterpreter();
		~SubInterpreter();

		public:
		virtual PyObject* Execute(PyObject* callable, PyObject* args, PyObject* kwArgs) override;
	};
} // namespace Pragmatic::auto_graph