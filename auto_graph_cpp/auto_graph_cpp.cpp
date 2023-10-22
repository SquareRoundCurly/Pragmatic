// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "Out.hpp"
#include "PythonUtils.hpp"
#include "GenericModule.hpp"

namespace Pragmatic::auto_graph
{
	auto_graph_cpp::auto_graph_cpp()
	{
		Out() << "[auto_graph] " << "Main module constructor" << std::endl;
	}

	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "cleanup" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::reinit(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "reinit" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::print(PyObject* self, PyObject* args)
	{
		const char* str;
		if (!PyArg_ParseTuple(args, "s", &str)) // Get a string
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be a string");
			return NULL;
		}

		Out() << str << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::add_task(PyObject* self, PyObject* args)
	{
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
		for (PyObject* task : tasks)
		{
			if (PyCallable_Check(task))
			{
				PyObject* result = PyObject_CallObject(task, nullptr);
				if (!result)
				{
					PyErr_Print();
				}
				Py_XDECREF(result);
			}
			Py_DECREF(task);
		}
		tasks.clear();

		Py_RETURN_NONE;
	}

	int auto_graph_cpp::init(PyObject *module)
	{
		Out() << "[auto_graph] " << "init" << std::endl;

		return 0;
	}

	int auto_graph_cpp::traverse(PyObject* module, visitproc visit, void* arg)
	{
		Out() << "[auto_graph] " << "traverse" << std::endl;

		return 0;
	}

	int auto_graph_cpp::clear(PyObject* module)
	{
		Out() << "[auto_graph] " << "clear" << std::endl;

		return 0;
	}

	void auto_graph_cpp::free(void* module)
	{
		Out() << "[auto_graph] " << "free" << std::endl;

		return;
	}
} // namespace Pragmatic::auto_graph