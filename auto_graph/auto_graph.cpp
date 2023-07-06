// Standard library
#include <iostream>

// External
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// auto_graph
#include "instrument.hpp"



int main()
{
	std::cout << "Hello world !" << std::endl;

	Py_SetPythonHome(CONCAT(L, PYTHON_ROOT_DIR));
	Py_Initialize();

	PyCompilerFlags cflags = {0};
	PyThreadState *substate, *mainstate;
	mainstate = PyThreadState_Get();

	PyThreadState_Swap(NULL);

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
		return NULL;
	}
	assert(substate != NULL);
	auto r = PyRun_SimpleStringFlags("print('Hello')", &cflags);
	Py_EndInterpreter(substate);

	PyThreadState_Swap(mainstate);

	Py_Finalize();

	return 0;
}