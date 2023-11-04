// Source header
#include "PyRef.hpp"

// External
#include "Python.h"

// auto_graph
#include "../Out.hpp"

namespace Pragmatic::auto_graph
{
	PyRef::PyRef() : obj(nullptr)
	{
	}

	PyRef::PyRef(PyObject *_obj) : obj(_obj)
	{
	}

	PyRef::PyRef(PyRef &&other) : obj(other.obj)
	{
		other.obj = nullptr;
	}
	PyRef &PyRef::operator=(PyRef &&other)
	{
		if (this != &other)
		{
			Py_XDECREF(obj);
			obj = other.obj;
			other.obj = nullptr;
		}
		return *this;
	}
	
	PyRef &PyRef::operator=(PyObject *_obj)
	{
		reset(_obj);
		return *this;
	}
	
	PyRef::~PyRef()
	{
		Out() << "~PyRef" << std::endl;
		Py_XDECREF(obj);
	}

	void PyRef::reset(PyObject *newObj)
	{
		Py_XDECREF(obj);
		obj = newObj;
	}
	
	void PyRef::IncRef()
	{
		Py_INCREF(obj);
	}
} // namespace Pragmatic::auto_graph