// Standard library
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

// auto_graph
#include "python_interpreter.hpp"
#include "streams.hpp"
#include "ScopeTimer.hpp"
#include "Graph.hpp"

typedef struct
{
	PyObject *an_object;
} module_state;

#define get_state(m) ((module_state*)PyModule_GetState(m))

static PyObject* initialize(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	SRC::auto_graph::Initialize();

	Py_RETURN_NONE;
}

static PyObject* task(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	PyObject* obj;

	if (!PyArg_ParseTuple(args, "O", &obj)) // Get an object
	{
		PyErr_SetString(PyExc_TypeError, "parameter must be a string or callable");
		return NULL;
	}

	if (PyUnicode_Check(obj)) // It's a string
	{
		const char* str = PyUnicode_AsUTF8(obj);
		auto strTask = std::string(str);
		std::filesystem::path pathTask(strTask);
		if (std::filesystem::exists(pathTask)) // It's a file
			SRC::auto_graph::AddTask(pathTask);
		else
			SRC::auto_graph::AddTask(strTask);
	}
	else if (PyCallable_Check(obj)) // It's a callable object
	{
		SRC::auto_graph::AddTask(obj);
	}
	else
	{
		PyErr_SetString(PyExc_TypeError, "parameter must be a string or callable");
		return NULL;
	}

	Py_RETURN_NONE;
}

static PyObject* cleanup(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	SRC::auto_graph::Cleanup();
	std::cout << "Cleaned up auto_graph" << std::endl;
	
	PROFILE_END_SESSION();

	Py_RETURN_NONE;
}

static PyObject* print(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	const char* str;
	if (!PyArg_ParseTuple(args, "s", &str)) // Get a string
    {
        PyErr_SetString(PyExc_TypeError, "parameter must be a string");
        return NULL;
    }

	SRC::auto_graph::Out() << str << std::endl;

	Py_RETURN_NONE;
}

static PyObject* testGraph(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	Test();

	Py_RETURN_NONE;
}

static PyMethodDef methods[] =
{
	{ "initialize", initialize, METH_NOARGS, "Initializes auto_graph_cpp & subinterpreters." },
	{ "task", task, METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
	{ "cleanup", cleanup, METH_NOARGS, "Cleanup function to be called before exit" },
	{ "print", print, METH_VARARGS, "A thread safe print function"},
	{ "testGraph", testGraph, METH_NOARGS, "test function" },
	{ NULL, NULL, 0, NULL }
};

static int ModuleInitialization(PyObject *module)
{
    // Module initialization logic goes here
    PROFILE_BEGIN_SESSION("auto_graph_profile.json");
    PROFILE_FUNCTION();

	SRC::auto_graph::Register(module);
    
    return 0;  // 0 for success, -1 for error (will cause import to fail)
}

static PyModuleDef_Slot slots[] =
{
    { Py_mod_exec, ModuleInitialization },
	{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
    { 0, NULL }
};

static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "auto_graph_cpp",      // name of module 
    NULL,                  // module documentation, may be NULL 
    sizeof(module_state),  // size of per-interpreter state of the module
    methods,
    slots,
    NULL,                  // m_traverse
    NULL,                  // m_clear
    NULL,                  // m_free
};

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
    return PyModuleDef_Init(&module);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}