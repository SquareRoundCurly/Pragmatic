// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "GenericModule.hpp"

namespace Pragmatic::auto_graph
{
	template <>
	auto_graph_cpp* GetModule(PyObject* module)
	{
		static PyObject* pyModule = nullptr;
		if (!pyModule) pyModule = module;
		return reinterpret_cast<auto_graph_cpp*>(PyModule_GetState((PyObject*)pyModule));
	}

	static PyMethodDef methods[] =
	{
		{ "cleanup", method<auto_graph_cpp, &auto_graph_cpp::cleanup>,  METH_NOARGS,  "Cleanup function to be called before exit"                      },
		{ "reinit",  method<auto_graph_cpp, &auto_graph_cpp::reinit>,   METH_NOARGS,  "Callback for module reloads"                                    },
		{ "print",   method<auto_graph_cpp, &auto_graph_cpp::print>,    METH_VARARGS, "A thread safe print function"                                   },
		{ "task",    method<auto_graph_cpp, &auto_graph_cpp::add_task>, METH_VARARGS, "Python functions to add to the executor pool"                   },
		{ "exec",    method<auto_graph_cpp, &auto_graph_cpp::exec>,     METH_VARARGS, "Execute added tasks"                                            },
		{ NULL, NULL, 0, NULL }
	};

	PyObject* create(PyModuleDef*);
	static PyModuleDef_Slot slots[] =
	{
		{ Py_mod_create, create },
		{ Py_mod_exec, init<auto_graph_cpp> },
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
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