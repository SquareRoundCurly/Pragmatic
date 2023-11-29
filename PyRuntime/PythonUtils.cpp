// Source header
#include "PythonUtils.hpp"

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	PyObject* ensure_tuple(PyObject* obj)
	{
		if (obj == NULL) return NULL;
		if (PyTuple_Check(obj)) return obj;

		// If obj is a single PyObject, wrap it in a tuple and return
		PyObject *single_item_tuple = PyTuple_Pack(1, obj);
		return single_item_tuple;
	}

	PyObject* merge_tuples(PyObject* args1, PyObject* args2)
	{
		if (!args1 && !args2)
		{
			return NULL;
		}
		else if (!args1)
		{
			// If args1 is NULL, check if args2 is a tuple and return it
			if (!PyTuple_Check(args2))
			{
				PyErr_SetString(PyExc_TypeError, "Expected tuple arguments");
				return NULL;
			}
			return args2;
		}
		else if (!args2)
		{
			// If args2 is NULL, check if args1 is a tuple and return it
			if (!PyTuple_Check(args1))
			{
				PyErr_SetString(PyExc_TypeError, "Expected tuple arguments");
				return NULL;
			}
			return args1;
		}

		if (!PyTuple_Check(args1) || !PyTuple_Check(args2))
		{
			PyErr_SetString(PyExc_TypeError, "Expected tuple arguments");
			return NULL;
		}

		Py_ssize_t size1 = PyTuple_Size(args1);
		Py_ssize_t size2 = PyTuple_Size(args2);
		PyObject* merged = PyTuple_New(size1 + size2);
		if (!merged)
		{
			return NULL;
		}

		for (Py_ssize_t i = 0; i < size1; i++)
		{
			PyObject* item = PyTuple_GetItem(args1, i);
			Py_INCREF(item);
			PyTuple_SetItem(merged, i, item);
		}

		for (Py_ssize_t i = 0; i < size2; i++)
		{
			PyObject* item = PyTuple_GetItem(args2, i);
			Py_INCREF(item);
			PyTuple_SetItem(merged, size1 + i, item);
		}

		return merged;
	}

	void ThrowPythonError(const std::string& message)
	{
		if (!PyErr_Occurred())
		{
			// Set the Python exception if there isn't already one set
			PyErr_SetString(PyExc_RuntimeError, message.c_str());
		}
	}
} // namespace Pragmatic::auto_graph