#pragma once

// auto_graph
#include "Interpreter.hpp"

namespace Pragmatic::auto_graph
{
	class MainInterpreter : public Interpreter
	{
		public:
		static MainInterpreter* Get();
		~MainInterpreter();
		private:
		MainInterpreter();

		public:
		virtual PyObject*  Execute(PyObject* callable, PyObject* args, PyObject* kwArgs) override;
	};
} // namespace Pragmatic::auto_graph