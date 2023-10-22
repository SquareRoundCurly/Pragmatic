#pragma once

// External
#include <pytypedefs.h>

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	class ModuleInterface
	{
		public: // Module callbacks
		virtual int init(PyObject *module) = 0;
		virtual int traverse(PyObject* module, visitproc visit, void* arg) = 0;
		virtual int clear(PyObject* module) = 0;
		virtual void free(void* module) = 0;
	};
} // namespace Pragmatic::auto_graph