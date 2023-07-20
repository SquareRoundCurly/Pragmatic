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

static struct PyModuleDef addmodule =
{
	PyModuleDef_HEAD_INIT,
	"auto_graph_cpp",      // name of module 
	NULL,                  // module documentation, may be NULL 
	-1,                    // size of per-interpreter state of the module, or -1 if the module keeps state in global variables. 
	methods
};

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	PROFILE_BEGIN_SESSION("auto_graph_profile.json");
	PROFILE_FUNCTION();

	SRC::auto_graph::Initialize();

	return PyModule_Create(&addmodule);
}

// Is never actially called
PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
	return PyInit_auto_graph_cpp();
}