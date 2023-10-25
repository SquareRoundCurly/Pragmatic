// Source header
#include "Task.hpp"

// External
#include "Python.h"

// auto_graph
#include "ClassRegistry.hpp"

namespace Pragmatic::auto_graph
{
	static int ClassInit(Task *self, PyObject *args, PyObject *kwds)
	{
		return 0;
	}

	static void ClassDestruct(Task* self)
	{
		return;
	}

	static PyTypeObject PyType_Pragmatic_auto_graph_Task =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Task",                    /* tp_name */
		sizeof(Task),                             /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)ClassDestruct,                /* tp_dealloc */
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
		"Task objects",                           /* tp_doc */
		0,                                        /* tp_traverse */
		0,                                        /* tp_clear */
		0,                                        /* tp_richcompare */
		0,                                        /* tp_weaklistoffset */
		0,                                        /* tp_iter */
		0,                                        /* tp_iternext */
		0,                                        /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		0,                                        /* tp_dictoffset */
		(initproc)ClassInit,                      /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	int Register(PyObject* module)
	{
		if (PyType_Ready(&PyType_Pragmatic_auto_graph_Task) < 0) return -1;

		Py_INCREF(&PyType_Pragmatic_auto_graph_Task);
		if (PyModule_AddObject(module, "Task", (PyObject *)&PyType_Pragmatic_auto_graph_Task) < 0)
		{
			Py_DECREF(&PyType_Pragmatic_auto_graph_Task);
			return -1;
    	}
	}

	REGISTER_CLASS(Register);
} // namespace Pragmatic::auto_graph