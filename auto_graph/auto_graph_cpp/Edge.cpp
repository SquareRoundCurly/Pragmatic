// Source header
#include "Edge.hpp"

// External
#include "Python.h"

// auto_graph
#include "Graph.hpp"

namespace SRC::auto_graph
{
	#pragma region Python

	typedef struct PyEdge
	{
		PyObject_HEAD;

		std::string source, target;
		Graph* graph;

		PyObject* __dict__;

		const Edge& GetEdge() { return graph->GetEdge(source, target); }
	};

	static void PyEdge_dealloc(PyEdge* self)
	{
		// Deallocate __dict__
		Py_XDECREF(self->__dict__);
		
		// Call the placement destructor for name
		self->source.~basic_string();
		self->target.~basic_string();
		
		// Free the object itself
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyObject* PyEdge_Exec(PyEdge* self)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Edge object not initialized");
			Py_RETURN_NONE;
		}

		if (!std::holds_alternative<std::monostate>(self->GetEdge().task.task))
			SRC::auto_graph::AddTask(self->GetEdge().task);

		Py_RETURN_NONE;
	}

	static PyObject* PyEdge_GetTaskResult(PyEdge* self)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Edge object not initialized");
			Py_RETURN_NONE;
		}

		try 
		{
			bool result = self->GetEdge().task.result->get_future().get();
			return PyBool_FromLong(result);
		}
		catch (const std::exception& e)
		{
			// If an exception was set on the promise, catch it and set Python error.
			PyErr_SetString(PyExc_RuntimeError, e.what());
			Py_RETURN_NONE;
		}
	}

	static PyMethodDef PyEdge_methods[] =
	{
		{ "__exec", (PyCFunction)PyEdge_Exec, METH_NOARGS, "Runs the stored task functor"},
		{ "get_result", (PyCFunction)PyEdge_GetTaskResult, METH_NOARGS, "A blocking call that waits for the task result & returns it" },
		{ nullptr }  // sentinel
	};

	static PyTypeObject PyEdgeType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Edge",                    /* tp_name */
		sizeof(PyEdge),                           /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyEdge_dealloc,               /* tp_dealloc */
		0,                                        /* tp_print */
		0,                                        /* tp_getattr */
		0,                                        /* tp_setattr */
		0,                                        /* tp_reserved */
		0,                                        /* tp_repr */
		0,                                        /* tp_as_number */
		0,                                        /* tp_as_sequence */
		0,                                        /* tp_as_mapping */
		0,                                        /* tp_hash  */
		0,                                        /* tp_call */
		0,                                        /* tp_str */
		PyObject_GenericGetAttr,                  /* tp_getattro */
		PyObject_GenericSetAttr,                  /* tp_setattro */
		0,                                        /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
		"Edge objects",                           /* tp_doc */
		0,                                        /* tp_traverse */
		0,                                        /* tp_clear */
		0,                                        /* tp_richcompare */
		0,                                        /* tp_weaklistoffset */
		0,                                        /* tp_iter */
		0,                                        /* tp_iternext */
		PyEdge_methods,                           /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		offsetof(PyEdge, __dict__),               /* tp_dictoffset */
		0,                                        /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	int Register_PyEdge(PyObject* module)
	{
		if (PyType_Ready(&PyEdgeType) < 0) return -1;

		Py_INCREF(&PyEdgeType);
		if (PyModule_AddObject(module, "Edge", (PyObject*)&PyEdgeType) < 0)
		{
			Py_DECREF(&PyEdgeType);
			return -1;
		}
	}

	#pragma endregion Python

	PyObject* CreatePyEdge(std::string source, std::string target, SRC::auto_graph::Graph* graph)
	{
		PyEdge* pyEdge = PyObject_New(PyEdge, &PyEdgeType);

		new (&pyEdge->source) std::string(source); // Placement new
		new (&pyEdge->target) std::string(target); // Placement new
		pyEdge->graph = graph;

		pyEdge->__dict__ = PyDict_New(); // Initialize __dict__
		if (!pyEdge->__dict__)
		{
			Py_DECREF(pyEdge);
			return nullptr;
		}

		return (PyObject*)pyEdge;
	}
} // namespace SRC::auto_graph