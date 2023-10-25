#pragma once

// External
#include <pytypedefs.h>

namespace Pragmatic::auto_graph
{
	class Task
	{
		Task(PyObject* callable, PyObject* args = nullptr);
		~Task();

		PyObject* Exec();
		PyObject* GetResult() const;

	private:
		PyObject* callable;
		PyObject* args;
		PyObject* result;
	};
} // namespace Pragmatic::auto_graph