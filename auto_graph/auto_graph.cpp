// Standard library
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

extern "C"
{
	int Add(int a, int b) { return a + b; }
}

static PyObject* add(PyObject* self, PyObject* args)
{
	int a;
	int b;

	if(!PyArg_ParseTuple(args, "ii", &a, &b)) return NULL;

	return Py_BuildValue("i", a + b);
}

static PyMethodDef methods[] =
{
	{ "add",  add, METH_VARARGS, "Add two numbers." },
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
	return PyModule_Create(&addmodule);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
	return PyModule_Create(&addmodule);
}