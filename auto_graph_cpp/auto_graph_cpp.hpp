#pragma once

// Standard library
#include <vector>
#include <array>

// External
#include <pytypedefs.h>

// auto_graph
#include "PyRuntime/Interfaces.hpp"
#include "PyRuntime/SubInterpreter.hpp"

namespace Pragmatic::auto_graph
{
	class auto_graph_cpp : public PyModule
	{
		public:
		auto_graph_cpp();
		~auto_graph_cpp();

		public:
		PyObject* cleanup(PyObject* self, PyObject* args);
		PyObject* reinit(PyObject* self, PyObject* args);
		PyObject* print(PyObject* self, PyObject* args);
		PyObject* add_task(PyObject* self, PyObject* args);
		PyObject* exec(PyObject* self, PyObject* args);

		PyObject* test(PyObject* self, PyObject* args);

		public:
		virtual int init(PyObject *module) override;
		virtual int traverse(PyObject* module, visitproc visit, void* arg) override;
		virtual int clear(PyObject* module) override;
		virtual void free(void* module) override;

		public:
		SubInterpreter* subInterpreter;
		private:
		std::vector<PyObject*> tasks;
	};
} // namespace Pragmatic::auto_graph