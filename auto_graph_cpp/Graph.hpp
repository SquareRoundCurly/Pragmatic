#pragma once

// Standard library
#include <vector>
#include <string>

// auto_graph
#include "Node.hpp"
#include "Edge.hpp"
#include "PyRuntime/PyClass.hpp"

namespace Pragmatic::auto_graph
{
	class Graph : public PyClass
	{
		public: // Constructors
		Graph();
		~Graph();

		public: // Python
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) override;
		virtual void PyClassDestruct(PyClass* self) override;

		public:
		PyObject* AddNode(PyObject* self, PyObject* args);

		private:
		std::vector<Node> nodes; // Store nodes in a vector
    	std::vector<Edge> edges;
	};
} // namespace Pragmatic::auto_graph