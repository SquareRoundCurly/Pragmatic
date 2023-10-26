#pragma once

// External
#include "Python.h"

// auto_graph
#include "Interfaces.hpp"

namespace Pragmatic::auto_graph
{
	template<class PyClassType>
	struct PyClassWrapper
	{
		PyObject_HEAD;
		PyClassType pyClass;
	};

	template <class PyClassDerived>
	using PyFunction = PyObject* (PyClassDerived::*)(PyObject*, PyObject*);

	template <class PyClassDerived, PyFunction<PyClassDerived> MethodPtr>
	static PyObject* Method(PyObject* self, PyObject* args)
	{
		return (((PyClassWrapper<PyClassDerived>*)self)->pyClass.*MethodPtr)(self, args);
	}

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