// Standard library
#include <iostream>
#include <thread>
#include <iostream>
#include <cstdio>
#include <array>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

// auto_graph
#include "python_interpreter.hpp"
#include "streams.hpp"
#include "ScopeTimer.hpp"
#include "Graph.hpp"
#include "Node.hpp"

typedef struct
{
	PyObject *an_object;
} module_state;

static PyObject* CustomWrite(PyObject* self, PyObject* args)
{
	const char* message;
	if (!PyArg_ParseTuple(args, "s", &message)) {
		return NULL;
	}

	SRC::auto_graph::Out() << message;

	Py_RETURN_NONE;
}

static PyObject* CustomFlush(PyObject* self, PyObject* args)
{
	Py_RETURN_NONE;
}

static PyMethodDef CustomMethods[] =
{
	{"write", CustomWrite, METH_VARARGS, "Custom write method"},
	{"flush", CustomFlush, METH_VARARGS, "Custom flush method"},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef CustomModule =
{
	PyModuleDef_HEAD_INIT,
	"custom",   // name of module
	NULL,       // module documentation, may be NULL
	-1,
	CustomMethods
};

static PyMethodDef CustomWriteMethod =
{
    "CustomWrite", // name of the python method
    (PyCFunction) CustomWrite, // C function pointer
    METH_VARARGS, // flag indicating the calling convention to be used
    "Custom write function" // docstring
};

static void RedirectPythonStandardOut()
{
	PyObject* sysModule = PyImport_ImportModule("sys");
	if (!sysModule)
	{
		PyErr_Print();
		return;
	}

	PyObject* customModule = PyModule_Create(&CustomModule);
	if (!customModule)
	{
		PyErr_Print();
		Py_DECREF(sysModule);
		return;
	}

	if (PyObject_SetAttrString(sysModule, "stdout", customModule) == -1)
	{
		PyErr_Print();
		Py_DECREF(customModule);
		Py_DECREF(sysModule);
		return;
	}

	Py_DECREF(customModule);
	Py_DECREF(sysModule);
}

static PyObject* initialize(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();

	RedirectPythonStandardOut();
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
			SRC::auto_graph::AddTask(SRC::auto_graph::PythonTask(pathTask));
		else
			SRC::auto_graph::AddTask(SRC::auto_graph::PythonTask(strTask));
	}
	else if (PyCallable_Check(obj)) // It's a callable object
	{
		SRC::auto_graph::AddTask(SRC::auto_graph::PythonTask(obj));
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
	SRC::auto_graph::Out() << "Cleaned up auto_graph" << std::endl;
	
	PROFILE_END_SESSION();

	Py_RETURN_NONE;
}

std::string exec(const char* cmd)
{
	PROFILE_FUNCTION();

	std::array<char, 128> buffer;
	std::string result;
	
	#ifdef _WIN32
        FILE* pipe = _popen(cmd, "r");
    #else
        FILE* pipe = popen(cmd, "r");
    #endif

	if (!pipe) throw std::runtime_error("popen() failed!");
	try
	{
		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			result += buffer.data();
		}
	}
	catch (...)
	{
		#ifdef _WIN32
            _pclose(pipe);
        #else
            pclose(pipe);
        #endif
		throw;
	}

	#ifdef _WIN32
        _pclose(pipe);
    #else
        pclose(pipe);
    #endif

	return result;
}

static PyObject* run_command(PyObject* self, PyObject* args)
{
	PROFILE_FUNCTION();
	
	const char* str;

	// Get a string for the command and an optional string for the cwd
	if (!PyArg_ParseTuple(args, "s", &str))
	{
		PyErr_SetString(PyExc_TypeError, "first parameter must be a string cmd, second parameter (optional) must be a string for cwd");
		return NULL;
	}
	
	auto output = exec(str);

	return PyUnicode_FromString(output.c_str());
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

static PyMethodDef methods[] =
{
	{ "initialize", initialize, METH_NOARGS, "Initializes auto_graph_cpp & subinterpreters." },
	{ "task", task, METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
	{ "cleanup", cleanup, METH_NOARGS, "Cleanup function to be called before exit" },
	{ "print", print, METH_VARARGS, "A thread safe print function"},
	{ "run_command", run_command, METH_VARARGS, "Runs a string shell command" },
	{ NULL, NULL, 0, NULL }
};

static int ModuleInitialization(PyObject *module)
{
    // Module initialization logic goes here
    PROFILE_BEGIN_SESSION("auto_graph_profile.json");
    PROFILE_FUNCTION();

	SRC::auto_graph::Register(module);
	SRC::auto_graph::Register_PyNode(module);
	SRC::auto_graph::Register_PyEdge(module);
	SRC::auto_graph::Register_PyGraph(module);

	RedirectPythonStandardOut();
	SRC::auto_graph::Initialize();
    
    return 0;  // 0 for success, -1 for error (will cause import to fail)
}

static PyModuleDef_Slot slots[] =
{
    { Py_mod_exec, (void*)ModuleInitialization },
	{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
    { 0, NULL }
};

static int auto_graph_traverse(PyObject* module, visitproc visit, void* arg)
{
    module_state* state = (module_state*)PyModule_GetState(module);
    Py_VISIT(state->an_object);
    return 0;
}

static int auto_graph_clear(PyObject* module)
{
    module_state* state = (module_state*)PyModule_GetState(module);
    Py_CLEAR(state->an_object);
    return 0;
}

void ModuleFree(void* module)
{
	std::cout << "Module freed" << std::endl;
}

static struct PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "auto_graph_cpp",      // name of module 
    NULL,                  // module documentation, may be NULL 
    sizeof(module_state),  // size of per-interpreter state of the module
    methods,
    slots,
    auto_graph_traverse,   // m_traverse
    auto_graph_clear,      // m_clear
    ModuleFree,            // m_free
};

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
    return PyModuleDef_Init(&module);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}