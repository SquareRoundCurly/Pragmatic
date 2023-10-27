// Source header
#include "Task.hpp"

// External
#include "Python.h"

// auto_graph
#include "Out.hpp"

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
	} else if (!args1)
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

		if (PyTuple_Size(args) < 1)
		{
			PyErr_SetString(PyExc_TypeError, "Expected at least a callable argument");
			return -1;
		}

		// Parse the first argument as the callable.
		callable = PyTuple_GetItem(args, 0);  // Note: This doesn't increase the reference count.
		if (!PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "First argument must be callable");
			return -1;
		}
		Py_INCREF(callable);

		// Create a new tuple for the rest of the arguments.
		this->args = PyTuple_GetSlice(args, 1, PyTuple_Size(args));
		if (this->args != NULL)
		{
			this->args = ensure_tuple(this->args);
			
			Py_INCREF(this->args);
		}

		return 0;
	}

	void Task::PyClassDestruct(PyClass* self)
	{
		Out() << "[auto_graph] Task destruct" << std::endl;

		if (callable) Py_DECREF(callable);
		if (args) Py_DECREF(args);
	}

	PyObject* Task::Exec(PyObject* self, PyObject* args)
	{
		auto* mergedArgs = merge_tuples(this->args, args);

		PyObject* funcCode = PyFunction_GetCode(callable);
		if (!funcCode)
		{
			Py_DECREF(mergedArgs);
			return NULL;  // Not a function or other error.
		}
		PyCodeObject* codeObj = (PyCodeObject*)funcCode;

    	Py_ssize_t expectedArgsCount = codeObj->co_argcount;
    	Py_ssize_t providedArgsCount = PyTuple_Size(mergedArgs);

		// Count default arguments
		PyObject* defaults = PyFunction_GetDefaults(callable);
		Py_ssize_t defaultArgsCount = defaults ? PyTuple_Size(defaults) : 0;

		if (providedArgsCount < (expectedArgsCount - defaultArgsCount))
		{
			PyErr_SetString(PyExc_TypeError, "Not enough arguments provided");
			Py_DECREF(mergedArgs);
			return NULL;
		}

		// If there are *args or **kwargs, we won't slice away excess arguments.
		bool hasVarArgs = codeObj->co_flags & CO_VARARGS;
		bool hasKeywordArgs = codeObj->co_flags & CO_VARKEYWORDS;
		if (!hasVarArgs && !hasKeywordArgs && providedArgsCount > expectedArgsCount)
		{
			PyObject* slicedArgs = PyTuple_GetSlice(mergedArgs, 0, expectedArgsCount);
			Py_DECREF(mergedArgs);
			mergedArgs = slicedArgs;
		}

		PyObject* result = PyObject_CallObject(callable, mergedArgs);
		Py_DECREF(mergedArgs);

		if (!result)
		{
			PyErr_Print();
		}

		return result;
	}
} // namespace Pragmatic::auto_graph