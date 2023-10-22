// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "GenericModule.hpp"

namespace Pragmatic::auto_graph
{
	static PyMethodDef methods[] =
	{
		{ "cleanup", method<auto_graph_cpp, &auto_graph_cpp::cleanup>,  METH_NOARGS,  "Cleanup function to be called before exit"                      },
		{ "reinit",  method<auto_graph_cpp, &auto_graph_cpp::reinit>,   METH_NOARGS,  "Callback for module reloads"                                    },
		{ "print",   method<auto_graph_cpp, &auto_graph_cpp::print>,    METH_VARARGS, "A thread safe print function"                                   },
		{ "task",    method<auto_graph_cpp, &auto_graph_cpp::add_task>, METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
		{ NULL, NULL, 0, NULL}
	};

	PyObject* create(PyModuleDef* moduleDef)
	{
		return PyModule_New(auto_graph_cpp::moduleDef.m_name);
	}

	static PyModuleDef_Slot slots[] =
	{
		{ Py_mod_create, create },
		{ Py_mod_exec, init<auto_graph_cpp> },
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
		{ 0, NULL }
	};

	PyModuleDef auto_graph_cpp::moduleDef =
	{
		PyModuleDef_HEAD_INIT,
		"auto_graph_cpp",                                 // name of module 
		NULL,                                             // module documentation, may be NULL 
		sizeof(auto_graph_cpp),                           // size of per-interpreter state of the module
		methods,                                          // methods
		slots,                                            // slots
		Pragmatic::auto_graph::traverse<auto_graph_cpp>,  // traverse
		Pragmatic::auto_graph::clear<auto_graph_cpp>,     // clear
		Pragmatic::auto_graph::free<auto_graph_cpp>       // free
	};
} // namespace Pragmatic::auto_graph

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	using namespace Pragmatic::auto_graph;

    return PyModuleDef_Init(&auto_graph_cpp::moduleDef);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}