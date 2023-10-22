#pragma once

// Standard library
#include <vector> 

// External
#include <pytypedefs.h>

// auto_graph
#include "ModuleInterface.hpp"

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	class auto_graph_cpp : public ModuleInterface
	{
		public:
		auto_graph_cpp();

		public:
		PyObject* cleanup(PyObject* self, PyObject* args);
		PyObject* reinit(PyObject* self, PyObject* args);
		PyObject* print(PyObject* self, PyObject* args);
		PyObject* add_task(PyObject* self, PyObject* args);

		public:
		virtual int init(PyObject *module) override;
		virtual int traverse(PyObject* module, visitproc visit, void* arg) override;
		virtual int clear(PyObject* module) override;
		virtual void free(void* module) override;

		private:
		std::vector<PyObject*> tasks;
	};
} // namespace Pragmatic::auto_graph