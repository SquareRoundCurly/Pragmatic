#pragma once

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	class PyClass
	{
		public: // PyObject callbacks
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) = 0;
		virtual void PyClassDestruct(PyClass* self) = 0;
	};

	template<class PyClassType>
	struct PyClassWrapper
	{
		PyObject_HEAD;
		PyClassType pyClass;
	};

	template <class PyClassDerived>
	int PyClassInit(PyClassWrapper<PyClassDerived>* self, PyObject* args, PyObject* kwds)
	{
		// Placement new, call constructor
		new (&self->pyClass) PyClassDerived();

		auto initReturnCode = self->pyClass.PyClassInit(&self->pyClass, args, kwds);

		return initReturnCode;
	}

	template <class PyClassDerived>
	void PyClassDestruct(PyClassWrapper<PyClassDerived>* self)
	{
		self->pyClass.PyClassDestruct(&self->pyClass);

		self->pyClass.~PyClassDerived();

		return;
	}
} // namespace Pragmatic::auto_graph