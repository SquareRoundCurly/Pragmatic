#pragma once

// auto_graph
#include "Interpreter.hpp"

#include "./../auto_graph_cpp/ThreadPool.hpp"

namespace Pragmatic::auto_graph
{
	class SubInterpreter : public Interpreter
	{
		public: // Ctor / Dtor
		SubInterpreter();
		~SubInterpreter();

		public:
		virtual PyObject* Execute(PyObject* callable, PyObject* args, PyObject* kwArgs) override;

		private:
		ThreadPool pool;
	};
} // namespace Pragmatic::auto_graph