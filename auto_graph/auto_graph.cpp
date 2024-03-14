#include <Python.h>

// Define a C++ function that will be exposed to Python
static PyObject* example_add(PyObject* self, PyObject* args)
{
    int a, b;
    if (!PyArg_ParseTuple(args, "ii", &a, &b))
	{
        return nullptr;
    }

    return PyLong_FromLong(a + b);
}

static PyMethodDef ExampleMethods[] =
{
    {"add",  example_add, METH_VARARGS, "Add two numbers together."},
    {nullptr, nullptr, 0, nullptr}  // Sentinel
};

// Module definition
static struct PyModuleDef auto_graph_module =
{
    PyModuleDef_HEAD_INIT,
    "auto_graph",   // Name of the module
    "Example module that adds two numbers together.", // Module documentation
    -1,          // Size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
    ExampleMethods
};

PyMODINIT_FUNC PyInit_auto_graph(void)
{
    return PyModule_Create(&auto_graph_module);
}