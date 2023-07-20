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

static PyObject* test(PyObject* self, PyObject* args)
{
	const char* str;
	if(!PyArg_ParseTuple(args, "s", &str)) return NULL;

	SRC::auto_graph::Test(str);
	
	Py_RETURN_NONE;
}

// Cleanup function
static PyObject* cleanup(PyObject* self, PyObject* args)
{
    std::cout << "Cleaning up auto_graph" << std::endl;
    
	PROFILE_END_SESSION();

    Py_RETURN_NONE;
}

static PyMethodDef methods[] =
{
	{ "test", test, METH_VARARGS, "Print 'test' to the console." },
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
	PROFILE_BEGIN_SESSION("auto_graph_cpp", "auto_grap_profile.json");
	return PyModule_Create(&addmodule);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
	PROFILE_BEGIN_SESSION("auto_graph_cpp", "auto_grap_profile.json");
	return PyModule_Create(&addmodule);
}