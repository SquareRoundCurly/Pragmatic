#pragma once

// External
#include <pytypedefs.h>

#include "PyRuntime/Interfaces.hpp"

namespace Pragmatic::auto_graph
{
	class Task : public PyClass
	{
		public:
		Task();
		~Task();

		public: // PyObject callbacks
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) override;
		virtual void PyClassDestruct(PyClass* self) override;

		public:
		PyObject* Exec(PyObject* self, PyObject* args);

		private:
		PyObject* callable = nullptr;
		PyObject* args = nullptr;
	};
} // namespace Pragmatic::auto_graph