// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>

// External
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// auto_graph
#include "instrument.hpp"

namespace SRC::AG
{
	namespace
	{
		void ReleaseGIL()
		{
			PyThreadState_Swap(NULL);
		}

		void SetupGlobals(PyThreadState* substate, long ID)
		{
			PyObject* main_module = nullptr;
			PyObject* global_dict = nullptr;
			PyObject* id = nullptr;

			PyThreadState_Swap(substate);

			main_module = PyImport_AddModule("__main__");
			if (main_module == NULL) goto error;

			global_dict = PyModule_GetDict(main_module);
			if (global_dict == NULL) goto error;

			id = PyLong_FromLong(ID);  // This is an int, not a long.
			if (id == NULL) goto error;

			// Set the integer as a global variable.
			if (PyDict_SetItemString(global_dict, "ID", id) < 0) goto error;

			// Decrease reference count for our integer object
			Py_DECREF(id);
			return;

			error: // Handle error & clean up any Python objects we created.
			Py_XDECREF(main_module);
			Py_XDECREF(global_dict);
			Py_XDECREF(id);
			return;
		}
	} // Anonymous namespace

	PythonInterpreter::PythonInterpreter()
	{
		PyConfig config;
		PyConfig_InitPythonConfig(&config);
		config.home = const_cast<wchar_t*>(CONCAT(L, PYTHON_ROOT_DIR));

		auto status = Py_InitializeFromConfig(&config);
		if (PyStatus_Exception(status))
		{
			std::cout << "Couldn't initialize Python" << std::endl;
		}

		mainstate = PyThreadState_Get();
	}

	PythonInterpreter::~PythonInterpreter()
	{
		isRunning = false;

		// End subinterpreters
		subinterpreters.clear();
		
		// End main interpreter
		PyThreadState_Swap(mainstate);
		Py_Finalize();
	}

	void PythonInterpreter::CreateSubinterpreter()
	{
		subinterpreters.emplace_back(std::make_unique<PythonSubinterpreter>(*this, subinterpreters.size()));
	}

	void PythonInterpreter::Run(int index, const std::string& script)
	{
		subinterpreters[index]->Run(script);
	}

	PythonSubinterpreter::PythonSubinterpreter(const PythonInterpreter& pythonInterpreter, const size_t index)
	: pythonInterpreter(pythonInterpreter), index(index)
	{
		const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
		
		ReleaseGIL();

		PyStatus status = Py_NewInterpreterFromConfig(&substate, &config);
		if (PyStatus_Exception(status))
		{
			/* Since no new thread state was created, there is no exception to
			propagate; raise a fresh one after swapping in the old thread
			state. */
			PyThreadState_Swap(pythonInterpreter.mainstate);
			_PyErr_SetFromPyStatus(status);
			PyObject *exc = PyErr_GetRaisedException();
			PyErr_SetString(PyExc_RuntimeError, "sub-interpreter creation failed");
			_PyErr_ChainExceptions1(exc);
			return;
		}

		assert(substate != NULL);

		SetupGlobals(substate, (long)index);
	}

	PythonSubinterpreter::~PythonSubinterpreter()
	{
		PyThreadState_Swap(substate);
		Py_EndInterpreter(substate);
	}

	void PythonSubinterpreter::Run(const std::string& script)
	{
		PyCompilerFlags cflags = {0};
		PyThreadState_Swap(substate);
		auto r = PyRun_SimpleStringFlags(script.c_str(), &cflags);
	}
}