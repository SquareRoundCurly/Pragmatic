// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "PyRuntime/PyModule.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "PyRuntime/ClassRegistry.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "PyRuntime/PyRef.hpp"

static PyObject* internal_runner(PyObject* self, PyObject* args)
{
	using namespace Pragmatic::auto_graph;

	PyObject* serialized_func;
	PyObject* serialized_args;

	if (!PyArg_ParseTuple(args, "OO", &serialized_func, &serialized_args))
		return NULL;

	// Load dill and its loads function
	PyRef dill_module = PyImport_ImportModule("dill");
	if (!dill_module)
		return NULL;

	PyRef loads_func = PyObject_GetAttrString(dill_module, "loads");
	if (!loads_func)
	{
		return NULL;
	}

	PyRef func = PyObject_CallFunctionObjArgs(loads_func, serialized_func, NULL);
	PyRef args_tuple = PyObject_CallFunctionObjArgs(loads_func, serialized_args, NULL);

	// Check if deserialization was successful
	if (!func || !args_tuple)
	{
		return NULL;
	}

	// Call the function
	PyRef result = PyObject_CallObject(func, args_tuple);

	// Serialize the result
	PyRef dumps_func = PyObject_GetAttrString(dill_module, "dumps");
	if (!dumps_func)
	{
		return NULL;
	}

	PyObject* serialized_result = PyObject_CallFunctionObjArgs(dumps_func, result.get(), NULL);

	return serialized_result;
}

namespace Pragmatic::auto_graph { PyObject* GetPyModule(PyObject* module = nullptr); }
static PyObject* run_in_subprocess(PyObject* self, PyObject* args)
{
	using namespace Pragmatic::auto_graph;

	PyObject* func;
	PyObject* args_tuple;
	
	// Parse the first argument as the callable.
	func = PyTuple_GetItem(args, 0);  // Note: This doesn't increase the reference count.
	if (!PyCallable_Check(func))
	{
		PyErr_SetString(PyExc_TypeError, "First argument must be callable");
		return NULL;
	}
	Py_INCREF(func);

	// Create a new tuple for the rest of the arguments.
	args_tuple = PyTuple_GetSlice(args, 1, PyTuple_Size(args));
	if (args_tuple != NULL)
	{
		args_tuple = Pragmatic::auto_graph::ensure_tuple(args_tuple);
		
		Py_INCREF(args_tuple);
	}

	// Load dill and its functions
	PyRef dill_module = PyImport_ImportModule("dill");
	if (!dill_module)
		return NULL;

	PyRef dumps_func = PyObject_GetAttrString(dill_module, "dumps");
	PyRef loads_func = PyObject_GetAttrString(dill_module, "loads");

	if (!dumps_func || !loads_func)
	{
		return NULL;
	}

	// Serialize the function and arguments
	PyRef serialized_func = PyObject_CallFunctionObjArgs(dumps_func, func, NULL);
	PyRef serialized_args = PyObject_CallFunctionObjArgs(dumps_func, args_tuple, NULL);

	// Use multiprocessing.Pool to run internal_runner
	PyRef multiprocessing_module = PyImport_ImportModule("multiprocessing");
	if (!multiprocessing_module)
	{
		return NULL;
	}

	const char* modulePath = "auto_graph/__private";
	const char* moduleName = "auto_graph_cpp";

	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
		return NULL;

	// Get sys.path
	PyRef sysPath = PyObject_GetAttrString(sysModule, "path");
	if (!sysPath)
	{
		return NULL;
	}

	PyRef pyModulePath = PyUnicode_FromString(modulePath);
	if (!pyModulePath)
	{
		return NULL;
	}

	if (PyList_Append(sysPath, pyModulePath) == -1)
	{
		return NULL;
	}

	PyRef module = PyImport_ImportModule(moduleName);
	if (!module)
		return NULL;

	PyObject* module_dict = PyModule_GetDict(module);
	PyObject* key, *value;
	Py_ssize_t pos = 0;

	while (PyDict_Next(module_dict, &pos, &key, &value)) {
		PyObject_Print(key, stdout, 0);
		PyObject_Print(value, stdout, 0);
		if (PyCallable_Check(value)) {
			const char* func_name = PyUnicode_AsUTF8(key);
			printf("Function name: %s\n", func_name);
		}
	}

	PyRef cfunc_internal_runner = PyObject_GetAttrString(module, "internal_runner");
	if (!cfunc_internal_runner)
	{
		return NULL;
	}

	PyRef Pool = PyObject_GetAttrString(multiprocessing_module, "Pool");
	PyRef pool = PyObject_CallFunction(Pool, "i", 1);  // Create a pool with one process

	PyRef func_args_tuple = PyTuple_Pack(2, serialized_func.get(), serialized_args.get());
	if (!func_args_tuple)
	{
		return NULL;
	}

	PyRef serialized_result = PyObject_CallMethod(pool, "apply", "OO", cfunc_internal_runner.get(), func_args_tuple.get());

	// Deserialize the result
	PyObject* result = PyObject_CallFunctionObjArgs(loads_func, serialized_result.get(), NULL);

	PyObject* close_method = PyObject_GetAttrString(pool, "close");
	if (close_method) {
		PyObject_CallObject(close_method, NULL);
		Py_DECREF(close_method);
	}

	PyObject* join_method = PyObject_GetAttrString(pool, "join");
	if (join_method) {
		PyObject_CallObject(join_method, NULL);
		Py_DECREF(join_method);
	}

	return result;
}

namespace Pragmatic::auto_graph
{
	PyObject* GetPyModule(PyObject* module)
	{
		static PyObject* pyModule = nullptr;
		if (!pyModule) pyModule = module;
		return pyModule;
	}

	template <>
	auto_graph_cpp* GetModule(PyObject* module)
	{
		auto* pyModule = GetPyModule(module);
		return reinterpret_cast<auto_graph_cpp*>(PyModule_GetState((PyObject*)pyModule));
	}

	static PyMethodDef methods[] =
	{
		{ "cleanup", method<auto_graph_cpp, &auto_graph_cpp::cleanup>,  METH_NOARGS,  "Cleanup function to be called before exit"                      },
		{ "reinit",  method<auto_graph_cpp, &auto_graph_cpp::reinit>,   METH_NOARGS,  "Callback for module reloads"                                    },
		{ "print",   method<auto_graph_cpp, &auto_graph_cpp::print>,    METH_VARARGS, "A thread safe print function"                                   },
		{ "task",    method<auto_graph_cpp, &auto_graph_cpp::add_task>, METH_VARARGS, "Python functions to add to the executor pool"                   },
		{ "exec",    method<auto_graph_cpp, &auto_graph_cpp::exec>,     METH_VARARGS, "Execute added tasks"                                            },
		
		{ "internal_runner", internal_runner, METH_VARARGS, "Internal runner for multiprocess runners" },
		{ "run_in_subprocess", run_in_subprocess, METH_VARARGS, "Runs callables in multiprocessing" },
		
		{ NULL, NULL, 0, NULL }
	};

	PyObject* create(PyModuleDef*);
	static PyModuleDef_Slot slots[] =
	{
		{ Py_mod_create, create },
		{ Py_mod_exec, init<auto_graph_cpp> },
		#if PY_VERSION_AT_LEAST(3, 12)
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
		#endif
		{ 0, NULL }
	};

	static PyModuleDef moduleDef =
	{
		PyModuleDef_HEAD_INIT,
		"auto_graph_cpp",                                // name of module 
		NULL,                                            // module documentation, may be NULL 
		sizeof(auto_graph_cpp),                          // size of per-interpreter state of the module
		methods,                                         // methods
		slots,                                           // slots
		Pragmatic::auto_graph::traverse<auto_graph_cpp>, // traverse
		Pragmatic::auto_graph::clear<auto_graph_cpp>,    // clear
		Pragmatic::auto_graph::free<auto_graph_cpp>      // free
	};

	PyObject* create(PyModuleDef* def)
	{
		PyObject* module = PyModule_New(moduleDef.m_name);

		return module;
	}
} // namespace Pragmatic::auto_graph

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	using namespace Pragmatic::auto_graph;

    return PyModuleDef_Init(&moduleDef);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}