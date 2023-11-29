// Source header
#include "Function.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRuntime/PythonUtils.hpp"

namespace Pragmatic::auto_graph
{
	Function::Function(PyObject* callable) : callable(callable)
	{
		
	}
	
	Function::Function(PyRef&& callable) : callable(std::move(callable))
	{

	}
	
	PyRef Function::operator()(PyObject* arg)
	{
		PyRef result = PyObject_CallFunctionObjArgs(callable, arg, NULL);

		if (!result)
			ThrowPythonError("Function did not return a valid value");

		return result;
	}
} // namespace Pragmatic::auto_graph