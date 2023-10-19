#pragma once

// External
#include <pytypedefs.h>

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	class auto_graph_cpp
	{
		public:
		static PyObject* cleanup(PyObject* self, PyObject* args);
		static PyObject* reinit(PyObject* self, PyObject* args);
		static PyObject* print(PyObject* self, PyObject* args);
		static PyObject* task(PyObject* self, PyObject* args);
		static int init(PyObject *module);
		static int traverse(PyObject* module, visitproc visit, void* arg);
		static int clear(PyObject* module);
		static void free(void* module);
	};
} // namespace Pragmatic::auto_graph