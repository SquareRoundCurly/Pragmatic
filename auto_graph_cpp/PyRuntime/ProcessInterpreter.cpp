// Source header
#include "ProcessInterpreter.hpp"

// External
#include "Python.h"

static PyObject* runner(PyObject* self, PyObject* args)
{
	PyObject* serialized_callable;
	PyObject* serialized_args;
	if (!PyArg_ParseTuple(args, "OO", &serialized_callable, &serialized_args))
	{
		return NULL;
	}

	PyObject* dill_module = PyImport_ImportModule("dill");
	if (!dill_module) return NULL;

	PyObject* loads_func = PyObject_GetAttrString(dill_module, "loads");
	if (!loads_func)
	{
		Py_DECREF(dill_module);
		return NULL;
	}

	PyObject* callable = PyObject_CallFunctionObjArgs(loads_func, serialized_callable, NULL);
	PyObject* args_tuple = PyObject_CallFunctionObjArgs(loads_func, serialized_args, NULL);
	if (!callable || !args_tuple)
	{
		Py_XDECREF(callable);
		Py_XDECREF(args_tuple);
		Py_DECREF(loads_func);
		Py_DECREF(dill_module);
		return NULL;
	}

	PyObject* result = PyObject_CallObject(callable, args_tuple);

	Py_DECREF(callable);
	Py_DECREF(args_tuple);
	Py_DECREF(loads_func);
	Py_DECREF(dill_module);

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
		PyObject* dill_module = PyImport_ImportModule("dill");
		if (!dill_module) return NULL;

		PyObject* dumps_func = PyObject_GetAttrString(dill_module, "dumps");
		if (!dumps_func) {
			Py_DECREF(dill_module);
			return NULL;
		}

		PyObject* serialized_callable = PyObject_CallFunctionObjArgs(dumps_func, callable, NULL);
		PyObject* serialized_args = PyObject_CallFunctionObjArgs(dumps_func, args, NULL);
		
		if (!serialized_callable || !serialized_args)
		{
			Py_XDECREF(serialized_callable);
			Py_XDECREF(serialized_args);
			Py_DECREF(dumps_func);
			Py_DECREF(dill_module);
			return NULL;
		}
		
		PyObject* mp_module = PyImport_ImportModule("multiprocessing");
		if (!mp_module)
		{
			Py_DECREF(serialized_callable);
			Py_DECREF(serialized_args);
			Py_DECREF(dumps_func);
			Py_DECREF(dill_module);
			return NULL;
		}

		PyObject* Pool_class = PyObject_GetAttrString(mp_module, "Pool");
		PyObject* pool = PyObject_CallObject(Pool_class, NULL);
		if (!pool)
		{
			Py_DECREF(serialized_callable);
			Py_DECREF(serialized_args);
			Py_DECREF(dumps_func);
			Py_DECREF(dill_module);
			Py_DECREF(Pool_class);
			Py_DECREF(mp_module);
			return NULL;
		}

		PyObject* result = PyObject_CallMethod(pool, "apply", "OOO", runner, serialized_callable, serialized_args);

		Py_DECREF(serialized_callable);
		Py_DECREF(serialized_args);
		Py_DECREF(dumps_func);
		Py_DECREF(dill_module);
		Py_DECREF(Pool_class);
		Py_DECREF(pool);
		Py_DECREF(mp_module);

		return result;
	}
} // namespace Pragmatic::auto_graph