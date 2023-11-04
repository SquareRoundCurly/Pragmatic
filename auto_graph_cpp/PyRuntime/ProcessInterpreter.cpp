// Source header
#include "ProcessInterpreter.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRef.hpp"
#include "PythonUtils.hpp"

#include "../PyNative/Module.hpp"

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
		auto dill = Module("dill");
		auto dumps = dill["dumps"];
		auto loads = dill["loads"];

		// Serialize the function and arguments
		PyRef serialized_func = dumps(callable);
		PyRef serialized_args = dumps(args);
		PyRef serialized_kw_args;
		if (kwArgs)
			serialized_kw_args = dumps(kwArgs);
		else
		{
			PyRef emptyDict = PyDict_New();
			serialized_kw_args = dumps(emptyDict);
		}

		// Use multiprocessing.Pool to run internal_runner
		PyRef multiprocessing_module = PyImport_ImportModule("multiprocessing");
		if (!multiprocessing_module)
			return NULL;

		const char* modulePath = "auto_graph/__private";
		const char* moduleName = "auto_graph_cpp";

		PyObject* sysModule = PyImport_ImportModule("sys");
		if (!sysModule)
			return NULL;

		// Get sys.path
		PyRef sysPath = PyObject_GetAttrString(sysModule, "path");
		if (!sysPath)
			return NULL;

		PyRef pyModulePath = PyUnicode_FromString(modulePath);
		if (!pyModulePath)
			return NULL;

		if (PyList_Append(sysPath, pyModulePath) == -1)
			return NULL;

		PyRef module = PyImport_ImportModule(moduleName);
		if (!module)
			return NULL;

		PyObject* module_dict = PyModule_GetDict(module);
		PyObject* key, *value;
		Py_ssize_t pos = 0;

		while (PyDict_Next(module_dict, &pos, &key, &value))
		{
			PyObject_Print(key, stdout, 0);
			PyObject_Print(value, stdout, 0);
			if (PyCallable_Check(value))
			{
				const char* func_name = PyUnicode_AsUTF8(key);
				printf("Function name: %s\n", func_name);
			}
		}

		// Get internal runner from new auto_graph_cpp instance
		PyRef cfunc_internal_runner = PyObject_GetAttrString(module, "internal_runner");
		if (!cfunc_internal_runner)
			return NULL;

		PyRef Pool = PyObject_GetAttrString(multiprocessing_module, "Pool");
		PyRef pool = PyObject_CallFunction(Pool, "i", 1);  // Create a pool with one process

		PyRef func_args_tuple = PyTuple_Pack(3, serialized_func.get(), serialized_args.get(), serialized_kw_args.get());
		if (!func_args_tuple)
			return NULL;

		// Run task
		PyRef serialized_result = PyObject_CallMethod(pool, "apply", "OO", cfunc_internal_runner.get(), func_args_tuple.get());

		// Deserialize the result
		auto result = loads(serialized_result);
		result.IncRef();
		// PyObject* result = PyObject_CallFunctionObjArgs(loads, serialized_result.get(), NULL);

		// Close the process
		PyObject* close_method = PyObject_GetAttrString(pool, "close");
		if (close_method)
		{
			PyObject_CallObject(close_method, NULL);
			Py_DECREF(close_method);
		}

		PyObject* join_method = PyObject_GetAttrString(pool, "join");
		if (join_method)
		{
			PyObject_CallObject(join_method, NULL);
			Py_DECREF(join_method);
		}

		return result;
	}
} // namespace Pragmatic::auto_graph