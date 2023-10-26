#pragma once

// External
#include <pytypedefs.h>

#include "PyRuntime/PyClass.hpp"

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
		PyObject* Exec();
		PyObject* GetResult() const;

		private:
		PyObject* callable;
		PyObject* args;
		PyObject* result;
	};
} // namespace Pragmatic::auto_graph