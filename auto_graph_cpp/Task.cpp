// Source header
#include "Task.hpp"

// External
#include "Python.h"

// auto_graph
#include "Out.hpp"

namespace Pragmatic::auto_graph
{
	Task::Task()
	{
		Out() << "[auto_graph] Task ctor" << std::endl;
	}

	Task::~Task()
	{
		Out() << "[auto_graph] Task dtor" << std::endl;
	}

	int Task::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		Out() << "[auto_graph] Task Init" << std::endl;

		return 0;
	}

	void Task::PyClassDestruct(PyClass* self)
	{
		Out() << "[auto_graph] Task destruct" << std::endl;
	}

	PyObject* Task::Exec()
	{
		Py_RETURN_NONE;
	}

	PyObject* Task::GetResult() const
	{
		Py_RETURN_NONE;
	}
} // namespace Pragmatic::auto_graph