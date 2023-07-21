// Standard library
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

// auto_graph
#include "python_interpreter.hpp"
#include "instrument.hpp"
#include "streams.hpp"

static PyObject* task(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	const char* str;
	if(!PyArg_ParseTuple(args, "s", &str)) return NULL;

	SRC::auto_graph::AddTask(str);
	
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

static PyMethodDef methods[] =
{
	{ "task", task, METH_VARARGS, "Print 'test' to the console." },
	{ "cleanup", cleanup, METH_NOARGS, "Cleanup function to be called before exit" },
	{ NULL, NULL, 0, NULL }
};

typedef struct
{
	PyObject *an_object;
} module_state;

#define get_state(m) ((module_state*)PyModule_GetState(m))

static int mymodule_exec(PyObject *module)
{
    // Module initialization logic goes here
    PROFILE_BEGIN_SESSION("auto_graph_profile.json");
    PROFILE_FUNCTION();
    SRC::auto_graph::Initialize();
    return 0;  // 0 for success, -1 for error (will cause import to fail)
}

static PyModuleDef_Slot mymodule_slots[] =
{
    { Py_mod_exec, mymodule_exec },
	{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
    { 0, NULL }
};

static struct PyModuleDef mymodule_def =
{
    PyModuleDef_HEAD_INIT,
    "auto_graph_cpp",      // name of module 
    NULL,                  // module documentation, may be NULL 
    sizeof(module_state),  // size of per-interpreter state of the module
    methods,
    mymodule_slots,
    NULL,                  // m_traverse
    NULL,                  // m_clear
    NULL,                  // m_free
};

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
    return PyModuleDef_Init(&mymodule_def);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}