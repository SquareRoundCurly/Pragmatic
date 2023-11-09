// Source header
#include "Graph.hpp"
#include "Out.hpp"
#include "Instrumentation.hpp"

namespace Pragmatic::auto_graph
{
	Graph::Graph()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph ctor" << std::endl;
	}

	Graph::~Graph()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph dtor" << std::endl;
	}

	int Graph::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph init" << std::endl;

		return 0;
	}

	void Graph::PyClassDestruct(PyClass* self)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph deinit" << std::endl;
	}
}

