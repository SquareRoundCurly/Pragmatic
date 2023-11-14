// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "Out.hpp"
#include "PyRuntime/MainInterpreter.hpp"
#include "PyRuntime/SubInterpreter.hpp"
#include "PyRuntime/ProcessInterpreter.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "PyRuntime/PyRef.hpp"
#include "Instrumentation.hpp"

namespace Pragmatic::auto_graph
{
	auto_graph_cpp::auto_graph_cpp()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "Main module constructor" << std::endl;

		interpreters.push_back(MainInterpreter::Get());
		interpreters.push_back(new SubInterpreter());
		interpreters.push_back(new ProcessInterpreter());
	}

	auto_graph_cpp::~auto_graph_cpp()
	{
		Out() << "[auto_graph] " << "Main module destructor" << std::endl;

		delete interpreters[1];
		delete interpreters[2];
		interpreters.clear();
	}

	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "cleanup" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::reinit(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "reinit" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::print(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		PyObject* obj;
		if (!PyArg_ParseTuple(args, "O", &obj)) // Get an object
		{
			PyErr_SetString(PyExc_TypeError, "Failed to parse argument as object");
			return NULL;
		}

		PyRef str_obj = PyObject_Str(obj); // Get the string representation of the object
		if (!str_obj)
		{
			return NULL;  // PyObject_Str failed.
		}

		const char* str = PyUnicode_AsUTF8(str_obj);
		if (!str)
		{
			Py_DECREF(str_obj);
			return NULL;  // PyUnicode_AsUTF8 failed.
		}

		Out() << str << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::add_task(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		PyObject* callable;
		if (!PyArg_ParseTuple(args, "O", &callable))
		{
			return nullptr;
		}

		Py_INCREF(callable);
		tasks.push_back(callable);

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::exec(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		for (PyRef task : tasks)
		{
			if (PyCallable_Check(task))
			{
				PyRef result = PyObject_CallObject(task, nullptr);
				if (!result)
				{
					PyErr_Print();
				}
			}
		}
		tasks.clear();

		Py_RETURN_NONE;
	}

	int auto_graph_cpp::init(PyObject *module)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "init" << std::endl;

		return 0;
	}

	int auto_graph_cpp::traverse(PyObject* module, visitproc visit, void* arg)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "traverse" << std::endl;

		return 0;
	}

	int auto_graph_cpp::clear(PyObject* module)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "clear" << std::endl;

		return 0;
	}

	void auto_graph_cpp::free(void* module)
	{
		PROFILE_FUNCTION();
		
		Out() << "[auto_graph] " << "free" << std::endl;

		return;
	}
} // namespace Pragmatic::auto_graph