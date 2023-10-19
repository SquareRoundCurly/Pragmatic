// Standard library
#include <iostream>

// External
#include <Python.h>
#define Py_BUILD_CORE
#include <internal/pycore_interp.h> // Interpreter state
#undef Py_BUILD_CORE

// auto_graph
#include "Out.hpp"
#include "State.hpp"
#include "PythonUtils.hpp"

namespace SRC::auto_graph
{
	static PyObject* cleanup(PyObject* self, PyObject* args)
	{
		Out() << "Cleanup" << std::endl;

		auto* state = AutoGraphState::Get();
		state->DecrementInitCounter();

		if (state->GetInitCounter() != 1)
			RaiseError("Mismatch between module inits and cleanups! Counter: " + std::to_string(state->GetInitCounter()));

		Py_RETURN_NONE;
	}

	static PyObject* reinit(PyObject* self, PyObject* args)
	{
		Out() << "Module re-init" << std::endl;

		auto* state = AutoGraphState::Get();
		state->IncrementInitCounter();

		Py_RETURN_NONE;
	}

	static PyObject* print(PyObject* self, PyObject* args)
	{
		const char* str;
		if (!PyArg_ParseTuple(args, "s", &str)) // Get a string
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be a string");
			return NULL;
		}

		SRC::auto_graph::Out() << str << std::endl;

		Py_RETURN_NONE;
	}

	static PyObject* task(PyObject* self, PyObject* args)
	{
		Py_RETURN_NONE;
	}

	static int interpreter_init(PyObject *module)
	{
		Out() << "Per-interpreter init" << std::endl;

		auto* state = AutoGraphState::Get();
		state->SetModule(module);
		state->IncrementInitCounter();

		return 0;  // 0 for success, -1 for error (will cause import to fail)
	}

	static int traverse(PyObject* module, visitproc visit, void* arg)
	{
		Out() << "Traverse" << std::endl;

		auto* state = InterpreterState::Get();
		Py_VISIT(state->an_object);

		return 0;
	}

	static int clear(PyObject* module)
	{
		Out() << "Clear" << std::endl;

		auto* interpreterState = InterpreterState::Get();
		Py_CLEAR(interpreterState->an_object);

		return 0;
	}

	void free(void* module)
	{
		auto* state = AutoGraphState::Get();
		state->DecrementInitCounter();
		
		if (state->GetInitCounter() != 0)
			Out() << "Mismatch between module inits and cleanups! Counter: " << state->GetInitCounter() << std::endl;

		Out() << "Module freed" << std::endl;
	}

	static PyMethodDef methods[] =
	{
		{ "cleanup", cleanup, METH_NOARGS, "Cleanup function to be called before exit" },
		{ "reinit", reinit, METH_NOARGS, "Callback function for handling module reloads" },
		{ "print", print, METH_VARARGS, "A thread safe print function"},
		{ "task", task, METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
		{ NULL, NULL, 0, NULL }
	};

	static PyModuleDef_Slot slots[] =
	{
		{ Py_mod_exec, (void*)interpreter_init },
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
		{ 0, NULL }
	};

	static struct PyModuleDef module =
	{
		PyModuleDef_HEAD_INIT,
		"auto_graph_cpp",         // name of module 
		NULL,                     // module documentation, may be NULL 
		sizeof(InterpreterState), // size of per-interpreter state of the module
		methods,                  // methods
		slots,                    // slots
		traverse,                 // traverse
		clear,                    // clear
		free,                     // free
	};
}


PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	using namespace SRC::auto_graph;

	PyInterpreterState* interpreter = PyThreadState_Get()->interp;
	Out() << "auto_graph init: " << interpreter->id << std::endl;

	auto* moduleObj = PyModuleDef_Init(&module);

	auto* state = AutoGraphState::Get();
	state->IncrementInitCounter();

    return moduleObj;
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}