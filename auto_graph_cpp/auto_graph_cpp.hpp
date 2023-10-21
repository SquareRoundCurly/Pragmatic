#pragma once

// Standard library
#include <vector> 

// External
#include <pytypedefs.h>

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	class auto_graph_cpp
	{
		public:
		PyObject* cleanup(PyObject* self, PyObject* args);
		PyObject* reinit(PyObject* self, PyObject* args);
		PyObject* print(PyObject* self, PyObject* args);
		PyObject* add_task(PyObject* self, PyObject* args);
		int init(PyObject *module);
		int traverse(PyObject* module, visitproc visit, void* arg);
		int clear(PyObject* module);
		void free(void* module);

		public:
		static PyModuleDef moduleDef;
		inline static PyObject* module = nullptr;

		private:
		std::vector<PyObject*> tasks;
	};
} // namespace Pragmatic::auto_graph