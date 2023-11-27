// Source header
#include "Graph.hpp"
#include "Out.hpp"
#include "Instrumentation.hpp"
#include "PyRuntime/PythonUtils.hpp"

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

	PyObject* Graph::AddNode(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		const char* name_cstr;
		if (!PyArg_ParseTuple(args, "s", &name_cstr))
		{
			ThrowPythonError("Expected a str name");
		}
		auto name = std::string(name_cstr);

		Out() << "[auto_graph] Added node: " << name << std::endl;

		Py_RETURN_NONE;
	}
}
