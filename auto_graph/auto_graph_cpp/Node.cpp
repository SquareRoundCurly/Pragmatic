// Source header
#include "Node.hpp"

// External
#include "Python.h"

// auto_graph
#include "python_interpreter.hpp"
#include "Graph.hpp"

namespace SRC::auto_graph
{
	Node::Node(const std::string& name) : name(name), task(std::monostate()) { }
	Node::Node(const std::string& name, PythonTask task) : name(name), task(task) { }

	#pragma region Python

	typedef struct PyNode
	{
		PyObject_HEAD;

		std::string name;
		Graph* graph;

		PyObject* __dict__;

		const Node& GetNode() { return graph->GetNode(name); }
	};

	static void PyNode_dealloc(PyNode* self)
	{
		// Deallocate __dict__
		Py_XDECREF(self->__dict__);
		
		// Call the placement destructor for name
		self->name.~basic_string();
		
		// Free the object itself
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyObject* PyNode_GetName(PyNode* self)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Node object not initialized");
			return nullptr;
		}

		return PyUnicode_FromString(self->GetNode().name.c_str());
	}

	static PyObject* PyNode_Exec(PyNode* self)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Node object not initialized");
			Py_RETURN_NONE;
		}

		if (!std::holds_alternative<std::monostate>(self->GetNode().task.task))
			SRC::auto_graph::AddTask(self->GetNode().task);

		Py_RETURN_NONE;
	}

	static PyObject* PyNode_GetTaskResult(PyNode* self)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Node object not initialized");
			Py_RETURN_NONE;
		}

		try 
		{
			bool result = self->GetNode().task.result->get_future().get();
			return PyBool_FromLong(result);
		}
		catch (const std::exception& e)
		{
			// If an exception was set on the promise, catch it and set Python error.
			PyErr_SetString(PyExc_RuntimeError, e.what());
			Py_RETURN_NONE;
		}
	}

	static PyMethodDef PyNode_methods[] =
	{
		{ "get_name", (PyCFunction)PyNode_GetName, METH_NOARGS, "Sorts the graph into topological generations & prints them" },
		{ "__exec", (PyCFunction)PyNode_Exec, METH_NOARGS, "Runs the stored task functor"},
		{ "get_result", (PyCFunction)PyNode_GetTaskResult, METH_NOARGS, "A blocking call that waits for the task result & returns it" },
		{ nullptr }  // sentinel
	};

	static PyTypeObject PyNodeType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Node",                    /* tp_name */
		sizeof(PyNode),                           /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyNode_dealloc,               /* tp_dealloc */
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
		"Node objects",                           /* tp_doc */
		0,                                        /* tp_traverse */
		0,                                        /* tp_clear */
		0,                                        /* tp_richcompare */
		0,                                        /* tp_weaklistoffset */
		0,                                        /* tp_iter */
		0,                                        /* tp_iternext */
		PyNode_methods,                           /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		offsetof(PyNode, __dict__),               /* tp_dictoffset */
		0,                                        /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	int Register_PyNode(PyObject* module)
	{
		if (PyType_Ready(&PyNodeType) < 0) return -1;

		Py_INCREF(&PyNodeType);
		if (PyModule_AddObject(module, "Node", (PyObject*)&PyNodeType) < 0)
		{
			Py_DECREF(&PyNodeType);
			return -1;
		}
	}

	PyObject* CreatePyNode(std::string name, SRC::auto_graph::Graph* graph)
	{
		PyNode* pyNode = PyObject_New(PyNode, &PyNodeType);

		new (&pyNode->name) std::string(name); // Placement new
		pyNode->graph = graph;

		pyNode->__dict__ = PyDict_New(); // Initialize __dict__
		if (!pyNode->__dict__)
		{
			Py_DECREF(pyNode);
			return nullptr;
		}

		return (PyObject*)pyNode;
	}

	Node GetNode(PyObject* pyNode)
	{
		if (PyUnicode_Check(pyNode))
		{
			const char* source = PyUnicode_AsUTF8(pyNode);
			return Node { std::string(source) };
		}
		else if (PyObject_TypeCheck(pyNode, &PyNodeType)) // Assuming PyNodeType is the type object for PyNode
		{
			return reinterpret_cast<PyNode*>(pyNode)->GetNode();
		}
		else
		{
			PyErr_SetString(PyExc_TypeError, "Source must be a string or a Node");
			return Node { "" };
		}
	}

	#pragma endregion Python
} // namespace SRC::auto_graph
