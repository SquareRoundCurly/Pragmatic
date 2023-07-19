// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>

// External
#include <Python.h>

namespace SRC::auto_graph
{
	void Test()
	{
		// Create a new sub-interpreter
		PyThreadState* mainInterpreterState = PyThreadState_Get();
		PyThreadState* subInterpreterState = Py_NewInterpreter();

		if (subInterpreterState == NULL)
		{
			return; // PyErr_NoMemory();
		}

		// Switch to the new interpreter
		PyThreadState_Swap(subInterpreterState);

		// Run the code
		int result = PyRun_SimpleString("print('hello from subinterpreter')");

		// Clean up the sub-interpreter
		Py_EndInterpreter(subInterpreterState);

		// Switch back to the main interpreter
		PyThreadState_Swap(mainInterpreterState);

		// Return the result as a Python object
		std::cout << PyLong_FromLong(result) << std::endl;
	}
} // namespace SRC::auto_graph
