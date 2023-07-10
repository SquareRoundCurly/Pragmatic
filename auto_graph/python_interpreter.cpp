// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>

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
		// End subinterpreters
		for (auto* substate : substates)
		{
			PyThreadState_Swap(substate);
			Py_EndInterpreter(substate);
		}
		
		// End main interpreter
		PyThreadState_Swap(mainstate);
		Py_Finalize();
	}

	void PythonInterpreter::CreateSubinterpreter()
	{
		ReleaseGIL();

		PyThreadState* substate;
		const PyInterpreterConfig config = _PyInterpreterConfig_INIT;

		PyStatus status = Py_NewInterpreterFromConfig(&substate, &config);
		if (PyStatus_Exception(status))
		{
			/* Since no new thread state was created, there is no exception to
			propagate; raise a fresh one after swapping in the old thread
			state. */
			PyThreadState_Swap(mainstate);
			_PyErr_SetFromPyStatus(status);
			PyObject *exc = PyErr_GetRaisedException();
			PyErr_SetString(PyExc_RuntimeError, "sub-interpreter creation failed");
			_PyErr_ChainExceptions1(exc);
			return;
		}

		assert(substate != NULL);

		// Get the main module.
		PyObject* main_module = PyImport_AddModule("__main__");
		if (main_module == NULL)
		{
			// Handle error...
			return;
		}

		// Get the dictionary that represents the module's global variables.
		PyObject* global_dict = PyModule_GetDict(main_module);
		if (global_dict == NULL)
		{
			// Handle error...
			return;
		}

		// Create an integer object.
		PyObject* id = PyLong_FromLong((long)substates.size());  // This is an int, not a long.
		if (id == NULL) {
			// Handle error...
			return;
		}

		// Set the integer as a global variable.
		if (PyDict_SetItemString(global_dict, "ID", id) < 0) {
			// Handle error...
			return;
		}

		// Decrease reference count for our integer object
		Py_DECREF(id);

		substates.push_back(substate);
	}

	void PythonInterpreter::Run(int index)
	{
		PyCompilerFlags cflags = {0};

		PyThreadState_Swap(substates[index]);
		auto r = PyRun_SimpleStringFlags("print(f'Hello {ID}')", &cflags);
	}
}