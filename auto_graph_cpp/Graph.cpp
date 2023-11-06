// Source header
#include "Graph.hpp"
#include "Out.hpp"

namespace Pragmatic::auto_graph
{
	Graph::Graph()
	{
		Out() << "[auto_graph] Graph ctor" << std::endl;
	}

	Graph::~Graph()
	{
		Out() << "[auto_graph] Graph dtor" << std::endl;
	}

	int Graph::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		Out() << "[auto_graph] Graph init" << std::endl;

		return 0;
	}

	void Graph::PyClassDestruct(PyClass* self)
	{
		Out() << "[auto_graph] Graph deinit" << std::endl;
	}
}

