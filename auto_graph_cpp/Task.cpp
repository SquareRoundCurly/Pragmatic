// Source header
#include "Task.hpp"

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	Task::Task(PyObject* callable, PyObject* args) : callable(callable), args(args), result(nullptr)
	{
		Py_XINCREF(callable);
		Py_XINCREF(args);
	}

	Task::~Task()
	{
		Py_XDECREF(callable);
		Py_XDECREF(args);
		Py_XDECREF(result);
	}

	PyObject* Task::Exec()
	{
		if (!PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "Stored object is not callable");
			return nullptr;
		}

		result = PyObject_CallObject(callable, args);
		return result;
	}

	PyObject* Task::GetResult() const
	{
		if (result)
		{
			Py_INCREF(result);
			return result;
		}
		
		Py_RETURN_NONE;
	}
} // namespace Pragmatic::auto_graph