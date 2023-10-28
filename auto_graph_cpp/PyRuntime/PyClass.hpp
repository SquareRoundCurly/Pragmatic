#pragma once

// External
#include "Python.h"

// auto_graph
#include "Interfaces.hpp"

namespace Pragmatic::auto_graph
{
	template<class PyClassDerived>
	struct PyClassWrapper
	{
		PyObject_HEAD;
		PyClassDerived pyClass;

		using PyFunction = PyObject* (PyClassDerived::*)(PyObject*, PyObject*);
		using PyFunctionNoArgs = PyObject* (PyClassDerived::*)(void);
		using PyFunctionKwArgs = PyObject* (PyClassDerived::*)(PyObject*, PyObject*, PyObject*);

		template <PyFunction MethodPtr>
		static PyObject* Method(PyObject* self, PyObject* args)
		{
			return (((PyClassWrapper<PyClassDerived>*)self)->pyClass.*MethodPtr)(self, args);
		}

		template <PyFunctionNoArgs MethodPtr>
		static PyObject* Method(PyObject* self, PyObject* args)
		{
			return (((PyClassWrapper<PyClassDerived>*)self)->pyClass.*MethodPtr)();
		}

		template <PyFunctionKwArgs MethodPtr>
		static PyObject* MethodKw(PyObject* self, PyObject* args, PyObject* kwArgs)
		{
			return (((PyClassWrapper<PyClassDerived>*)self)->pyClass.*MethodPtr)(self, args, kwArgs);
		}

		static int PyClassInit(PyClassWrapper<PyClassDerived>* self, PyObject* args, PyObject* kwds)
		{
			// Task original = *self;
			// new (self) Task();
			// self->ob_base = original.ob_base;

			// Placement new, call constructor
			new (&self->pyClass) PyClassDerived();

			auto initReturnCode = self->pyClass.PyClassInit(&self->pyClass, args, kwds);

			return initReturnCode;
		}

		static void PyClassDestruct(PyClassWrapper<PyClassDerived>* self)
		{
			self->pyClass.PyClassDestruct(&self->pyClass);

			self->pyClass.~PyClassDerived();

			return;
		}
	};
} // namespace Pragmatic::auto_graph