// Source header
#include "Node.hpp"

// External
#include "Python.h"

// auto_graph
#include "python_interpreter.hpp"

namespace SRC::auto_graph
{
	Node::Node(const std::string& name) : id(name), task(std::string()) { }
	Node::Node(const std::string& name, PythonTask task) : id(name), task(task) { }

	#pragma region Python

	typedef struct PyNode
	{
		PyObject_HEAD;
		Node* node;
	};

	static int PyNode_init(PyNode *self, PyObject *args, PyObject *kwds)
	{
		char* c_name;
		PyObject* obj = nullptr; // Initialize to nullptr to handle the optional argument

		// Parse the first arg, and optionally the second arg
		if (!PyArg_ParseTuple(args, "s|O", &c_name, &obj)) return -1;

		std::string name(c_name);
		self->node = new Node(name);

		// If there is a second arg
		if (obj)
		{
			if (PyUnicode_Check(obj)) // It's a string
			{
				const char* str = PyUnicode_AsUTF8(obj);
				auto strTask = std::string(str);
				std::filesystem::path pathTask(strTask);
				if (std::filesystem::exists(pathTask)) // It's a file
					self->node->task = pathTask;
				else
					self->node->task = strTask;
			}
			else if (PyCallable_Check(obj)) // It's a callable object
			{
				self->node->task = obj;
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "Second parameter must be a string or callable");
				return -1;
			}
		}

		return 0;
	}

	static void PyNode_dealloc(PyNode* self)
	{
		delete self->node;
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyObject* PyNode_GetName(PyNode* self)
	{
		if (!self->node)
		{
			PyErr_SetString(PyExc_RuntimeError, "Node object not initialized");
			return nullptr;
		}

		return PyUnicode_FromString(self->node->id.c_str());
	}

	static void PyNode_Exec(PyNode* self)
	{
		if (!self->node)
		{
			PyErr_SetString(PyExc_RuntimeError, "Node object not initialized");
			return;
		}

		SRC::auto_graph::AddTask(self->node->task);
	}

	static PyMethodDef PyNode_methods[] =
	{
		{ "get_name", (PyCFunction)PyNode_GetName, METH_NOARGS, "Sorts the graph into topological generations & prints them" },
		{ "__exec", (PyCFunction)PyNode_Exec, METH_NOARGS, "Runs the stored task functor"},
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
		0,                                        /* tp_getattro */
		0,                                        /* tp_setattro */
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
		0,                                        /* tp_dictoffset */
		(initproc)PyNode_init,                    /* tp_init */
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

	PyObject* CreatePyNode(const Node& node)
	{
		PyNode* pyNode = PyObject_New(PyNode, &PyNodeType);
		pyNode->node = new Node(node); // Copy node

		return (PyObject*)pyNode;
	}

	#pragma endregion Python
} // namespace SRC::auto_graph
