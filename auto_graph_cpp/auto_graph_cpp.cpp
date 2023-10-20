// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "Out.hpp"
#include "PythonUtils.hpp"

namespace Pragmatic::auto_graph
{
	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "cleanup" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::reinit(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "reinit" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::print(PyObject* self, PyObject* args)
	{
		const char* str;
		if (!PyArg_ParseTuple(args, "s", &str)) // Get a string
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be a string");
			return NULL;
		}

		Pragmatic::auto_graph::Out() << str << std::endl;

		Py_RETURN_NONE;

	}

	PyObject* auto_graph_cpp::task(PyObject* self, PyObject* args)
	{
		
		Py_RETURN_NONE;
	}

	int auto_graph_cpp::init(PyObject *module)
	{
		Out() << "[auto_graph] " << "init" << std::endl;

		return 0;
	}

	int auto_graph_cpp::traverse(PyObject* module, visitproc visit, void* arg)
	{
		Out() << "[auto_graph] " << "traverse" << std::endl;

		return 0;
	}

	int auto_graph_cpp::clear(PyObject* module)
	{
		Out() << "[auto_graph] " << "clear" << std::endl;

		return 0;
	}

	void auto_graph_cpp::free(void* module)
	{
		Out() << "[auto_graph] " << "free" << std::endl;

		return;
	}

	static PyMethodDef methods[] =
	{
		{ "cleanup", auto_graph_cpp::cleanup, METH_NOARGS,  "Cleanup function to be called before exit"                      },
		{ "reinit",  auto_graph_cpp::reinit,  METH_NOARGS, "Callback for module reloads"                                     },
		{ "print",   auto_graph_cpp::print,   METH_VARARGS, "A thread safe print function"                                   },
		{ "task",    auto_graph_cpp::task,    METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
		{ NULL,      NULL,                    0,            NULL                                                             }
	};

	static PyModuleDef_Slot slots[] =
	#if PY_VERSION_AT_LEAST(3, 12)
	{
		{ Py_mod_exec, (void*)auto_graph_cpp::init },
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
		{ 0, NULL }
	};
	#else
	{
		{ Py_mod_exec, (void*)auto_graph_cpp::init },
		{ 0, NULL }
	};
	#endif

	static struct PyModuleDef module =
	{
		PyModuleDef_HEAD_INIT,
		"auto_graph_cpp",         // name of module 
		NULL,                     // module documentation, may be NULL 
		sizeof(auto_graph_cpp),   // size of per-interpreter state of the module
		methods,                  // methods
		slots,                    // slots
		auto_graph_cpp::traverse, // traverse
		auto_graph_cpp::clear,    // clear
		auto_graph_cpp::free,     // free
	};
} // namespace Pragmatic::auto_graph

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	using namespace Pragmatic::auto_graph;

    return PyModuleDef_Init(&module);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}