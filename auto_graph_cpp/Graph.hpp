#pragma once

// auto_graph
#include "PyRuntime/PyClass.hpp"

namespace Pragmatic::auto_graph
{
	class Graph : public PyClass
	{
		public: // Constructors
		Graph();
		~Graph();

		public:
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) override;
		virtual void PyClassDestruct(PyClass* self) override;
	};
} // namespace Pragmatic::auto_graph