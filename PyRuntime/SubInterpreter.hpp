#pragma once

// auto_graph
#include "./../auto_graph_cpp/ThreadPool.hpp"

namespace Pragmatic::auto_graph
{
	class SubInterpreter
	{
		public: // Ctor / Dtor
		SubInterpreter();
		~SubInterpreter();

		public:
		PyObject* Execute(PyObject* callable, PyObject* args, PyObject* kwArgs);

		private:
		ThreadPool pool;
	};
} // namespace Pragmatic::auto_graph