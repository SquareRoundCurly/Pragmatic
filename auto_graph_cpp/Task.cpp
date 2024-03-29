// Source header
#include "Task.hpp"

// External
#include "Python.h"

// auto_graph
#include "Out.hpp"
#include "auto_graph_cpp.hpp"
#include "PyRuntime/PyModule.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "PyRuntime/PyRef.hpp"
#include "Instrumentation.hpp"

namespace Pragmatic::auto_graph
{
	Task::Task()
	{
		PROFILE_FUNCTION();
	}

	Task::~Task()
	{
		PROFILE_FUNCTION();
	}

	int Task::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		PROFILE_FUNCTION();

		if (PyTuple_Size(args) < 1)
		{
			PyErr_SetString(PyExc_TypeError, "Expected at least a callable argument");
			return -1;
		}

		// Parse the first argument as the callable.
		callable = PyTuple_GetItem(args, 0);  // Note: This doesn't increase the reference count.
		if (!PyCallable_Check(callable))
		{
			PyErr_SetString(PyExc_TypeError, "First argument must be callable");
			return -1;
		}
		Py_INCREF(callable);

		// Create a new tuple for the rest of the arguments.
		this->args = PyTuple_GetSlice(args, 1, PyTuple_Size(args));
		if (this->args != NULL)
		{
			this->args = ensure_tuple(this->args);
			
			Py_INCREF(this->args);
		}

		return 0;
	}

	void Task::PyClassDestruct(PyClass* self)
	{
		PROFILE_FUNCTION();

		if (callable) Py_DECREF(callable);
		if (args) Py_DECREF(args);
	}

	PyObject* Task::Exec(PyObject* self, PyObject* args, PyObject* kwargs)
	{
		PROFILE_FUNCTION();

		PyRef mergedArgs = merge_tuples(this->args, args);

		PyObject* funcCode = PyFunction_GetCode(callable);
		if (!funcCode)
			return NULL; // Not a function or other error.

		PyCodeObject* codeObj = (PyCodeObject*)funcCode;

    	Py_ssize_t expectedArgsCount = codeObj->co_argcount;
    	Py_ssize_t providedArgsCount = PyTuple_Size(mergedArgs);

		// Count default arguments
		PyObject* defaults = PyFunction_GetDefaults(callable);
		Py_ssize_t defaultArgsCount = defaults ? PyTuple_Size(defaults) : 0;

		if (providedArgsCount < (expectedArgsCount - defaultArgsCount))
		{
			PyErr_SetString(PyExc_TypeError, "Not enough arguments provided");
			return NULL;
		}

		// If there are *args or **kwargs, we won't slice away excess arguments.
		bool hasVarArgs = codeObj->co_flags & CO_VARARGS;
		bool hasKeywordArgs = codeObj->co_flags & CO_VARKEYWORDS;
		if (!hasVarArgs && !hasKeywordArgs && providedArgsCount > expectedArgsCount)
		{
			PyObject* slicedArgs = PyTuple_GetSlice(mergedArgs, 0, expectedArgsCount);
			mergedArgs = slicedArgs;
		}

		PyObject_Print(mergedArgs, stdout, 0);

		PyObject* result = GetModule<auto_graph_cpp>()->subInterpreter->Execute(callable, mergedArgs, kwargs).get();

		if (!result)
		{
			PyErr_Print();
		}

		return result;
	}

    TaskFuture<std::function<PyObject *()>> Task::ExecFuture(PyObject *self, PyObject *args, PyObject *kwargs)
    {
        PROFILE_FUNCTION();

		PyRef mergedArgs = merge_tuples(this->args, args);

		PyObject* funcCode = PyFunction_GetCode(callable);
		if (!funcCode)
			return CreateTaskAndFuture([]() -> PyObject* { return NULL; }).second; // Not a function or other error.

		PyCodeObject* codeObj = (PyCodeObject*)funcCode;

    	Py_ssize_t expectedArgsCount = codeObj->co_argcount;
    	Py_ssize_t providedArgsCount = PyTuple_Size(mergedArgs);

		// Count default arguments
		PyObject* defaults = PyFunction_GetDefaults(callable);
		Py_ssize_t defaultArgsCount = defaults ? PyTuple_Size(defaults) : 0;

		if (providedArgsCount < (expectedArgsCount - defaultArgsCount))
		{
			PyErr_SetString(PyExc_TypeError, "Not enough arguments provided");
			return CreateTaskAndFuture([]() -> PyObject* { return NULL; }).second;
		}

		// If there are *args or **kwargs, we won't slice away excess arguments.
		bool hasVarArgs = codeObj->co_flags & CO_VARARGS;
		bool hasKeywordArgs = codeObj->co_flags & CO_VARKEYWORDS;
		if (!hasVarArgs && !hasKeywordArgs && providedArgsCount > expectedArgsCount)
		{
			PyObject* slicedArgs = PyTuple_GetSlice(mergedArgs, 0, expectedArgsCount);
			mergedArgs = slicedArgs;
		}

		PyObject_Print(mergedArgs, stdout, 0);

		auto result = GetModule<auto_graph_cpp>()->subInterpreter->Execute(callable, mergedArgs, kwargs);
		return result;
    }
} // namespace Pragmatic::auto_graph