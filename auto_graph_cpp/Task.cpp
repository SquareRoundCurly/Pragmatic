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
		Out() << "[auto_graph] Task init" << std::endl;

		if (!PyArg_ParseTuple(args, "O", &callable))
		{
			PyErr_SetString(PyExc_TypeError, "Expected an callable argument");
			return -1;
		}
		if (PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "Expected an callable argument");
			return -1;
		}

		Py_INCREF(callable);

		return 0;
	}

	void Task::PyClassDestruct(PyClass* self)
	{
		Out() << "[auto_graph] Task destruct" << std::endl;

		if (callable) Py_DECREF(callable);
	}

	PyObject* Task::Exec()
	{

		PyObject* result = PyObject_CallObject(callable, nullptr);
		if (!result)
		{
			PyErr_Print();
		}
		Py_XDECREF(result);

		return result;
	}
} // namespace Pragmatic::auto_graph