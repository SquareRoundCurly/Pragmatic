#pragma once

// External
#include <pytypedefs.h>

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	class PyModule
	{
		public: // Module callbacks
		virtual int init(PyObject *module) = 0;
		virtual int traverse(PyObject* module, visitproc visit, void* arg) = 0;
		virtual int clear(PyObject* module) = 0;
		virtual void free(void* module) = 0;
	};

	class PyClass
	{
		public: // PyObject callbacks
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) = 0;
		virtual void PyClassDestruct(PyClass* self) = 0;
	};
}