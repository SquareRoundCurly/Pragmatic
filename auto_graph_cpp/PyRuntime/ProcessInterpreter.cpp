// Source header
#include "ProcessInterpreter.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRef.hpp"

static PyObject* runner(PyObject* self, PyObject* args)
{
	using namespace Pragmatic::auto_graph;

	PyObject* serialized_callable;
	PyObject* serialized_args;
	if (!PyArg_ParseTuple(args, "OO", &serialized_callable, &serialized_args))
	{
		return NULL;
	}

	PyRef dill_module = PyImport_ImportModule("dill");
	if (!dill_module) return NULL;

	PyRef loads_func = PyObject_GetAttrString(dill_module, "loads");
	if (!loads_func)
	{
		// Py_DECREF(dill_module);
		return NULL;
	}

	PyRef callable = PyObject_CallFunctionObjArgs(loads_func, serialized_callable, NULL);
	PyRef args_tuple = PyObject_CallFunctionObjArgs(loads_func, serialized_args, NULL);
	if (!callable || !args_tuple)
	{
		// Py_XDECREF(callable);
		// Py_XDECREF(args_tuple);
		// Py_DECREF(loads_func);
		// Py_DECREF(dill_module);
		return NULL;
	}

	PyObject* result = PyObject_CallObject(callable, args_tuple);

	// Py_DECREF(callable);
	// Py_DECREF(args_tuple);
	// Py_DECREF(loads_func);
	// Py_DECREF(dill_module);

	return result;
}

namespace Pragmatic::auto_graph
{
	ProcessInterpreter::ProcessInterpreter()
	{

	}

	ProcessInterpreter::~ProcessInterpreter()
	{

	}

	PyObject* ProcessInterpreter::Execute(PyObject* callable, PyObject* args, PyObject* kwArgs)
	{
		PyRef dill_module = PyImport_ImportModule("dill");
		if (!dill_module) return NULL;

		PyRef dumps_func = PyObject_GetAttrString(dill_module, "dumps");
		if (!dumps_func) {
			Py_DECREF(dill_module);
			return NULL;
		}

		PyRef serialized_callable = PyObject_CallFunctionObjArgs(dumps_func, callable, NULL);
		PyRef serialized_args = PyObject_CallFunctionObjArgs(dumps_func, args, NULL);
		
		if (!serialized_callable || !serialized_args)
		{
			return NULL;
		}
		
		PyRef mp_module = PyImport_ImportModule("multiprocessing");
		if (!mp_module)
		{
			return NULL;
		}

		PyRef Pool_class = PyObject_GetAttrString(mp_module, "Pool");
		PyRef pool = PyObject_CallObject(Pool_class, NULL);
		if (!pool)
		{
			return NULL;
		}

		PyObject* result = PyObject_CallMethod(pool.get(), "apply", "OOO", runner, serialized_callable.get(), serialized_args.get());

		return result;
	}
} // namespace Pragmatic::auto_graph