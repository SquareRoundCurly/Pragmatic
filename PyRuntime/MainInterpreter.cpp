// Source header
#include "MainInterpreter.hpp"

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	MainInterpreter* MainInterpreter::Get()
	{
		static MainInterpreter mainInterpreter;
		return &mainInterpreter;
	}

	MainInterpreter::MainInterpreter()
	{

	}
	
	MainInterpreter::~MainInterpreter()
	{
		
	}
	
	PyObject* MainInterpreter::Execute(PyObject* callable, PyObject* args, PyObject* kwArgs)
	{
		if (!PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "Expected a callable");
			return NULL;
   		}
    
    	PyObject* result = PyObject_Call(callable, args, kwArgs);

		return result;
	}
	
} // namespace Pragmatic::auto_graph