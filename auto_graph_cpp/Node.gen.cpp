// Source header
#include "Node.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRuntime/ClassRegistry.hpp"
#include "PyRuntime/PythonUtils.hpp"

namespace Pragmatic::auto_graph
{
	int PyInit(PyNode* self, PyObject* args, PyObject* kwds)
	{
		const char* name_cstr;

		if (!PyArg_ParseTuple(args, "s", &name_cstr))
		{
			ThrowPythonError("Expected a str name");
		}

		self->node = new Node(name_cstr);

		return 0;
	}

	void PyDestruct(PyNode* self)
	{
		if (self->owner == Owner::PY)
			delete self->node;
	}

	PyObject *PyNode_Name(PyNode* self, PyObject* Py_UNUSED(ignored))
	{
		const char* result = self->node->name.c_str();
		return PyUnicode_FromString(result);
	}

	static PyMethodDef PyNode_methods[] =
	{
		{ "name", (PyCFunction) PyNode_Name, METH_NOARGS, "Get name of node" },
		{ nullptr } // sentinel
	};

	static PyTypeObject PyType_Pragmatic_auto_graph_Node =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Node",                    /* tp_name */
		sizeof(PyNode),                           /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyDestruct,                   /* tp_dealloc */
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
		(initproc)PyInit,                         /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};
	
	PyTypeObject* GetPyTypeObject()
	{
		return &PyType_Pragmatic_auto_graph_Node;
	}

	REGISTER_CLASS(PyType_Pragmatic_auto_graph_Node, Node);
} // namespace Pragmatic::auto_graph