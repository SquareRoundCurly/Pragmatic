// Source header
#include "SubInterpreter.hpp"

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	SubInterpreter::SubInterpreter()
	{
		
	}
	
	SubInterpreter::~SubInterpreter()
	{
		
	}
	
	PyObject* SubInterpreter::Execute(PyObject* callable, PyObject* args, PyObject* kwArgs)
	{
		if (!PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "Expected a callable");
			return NULL;
		}
	
		PyThreadState* mainState = PyThreadState_Get();
		PyThreadState* newState = Py_NewInterpreter();
		if (!newState)
		{
			return NULL;
		}
		
		PyObject* result = PyObject_Call(callable, args, kwArgs);

		Py_EndInterpreter(newState);
		PyThreadState_Swap(mainState);

		return result;
	}
} // namespace Pragmatic::atuo_graph