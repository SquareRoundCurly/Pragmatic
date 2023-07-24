// Source header
#include "Instrumentation.hpp"

// Python
#include <Python.h>

// auto_graph
#include "../streams.hpp"

extern "C"
{

}

namespace SRC::auto_graph
{
	ScopeTimer::ScopeTimer(const std::string& name) : name(name)
	{
		Out() << "ScopeTimer " << name << std::endl;
	}

	ScopeTimer::~ScopeTimer()
	{
		Out() << "~ScopeTimer " << name << std::endl;
	}

	typedef struct
	{
		PyObject_HEAD
		SRC::auto_graph::ScopeTimer* scopeTimer;
	} PyScopeTimer;

	static int PyScopeTimer_init(PyScopeTimer *self, PyObject *args, PyObject *kwds)
	{
		char* c_name;

		if (!PyArg_ParseTuple(args, "s", &c_name)) return -1;

		std::string name(c_name);
		self->scopeTimer = new ScopeTimer(name);

		return 0;
	}

	static void PyScopeTimer_dealloc(PyScopeTimer* self)
	{
		delete self->scopeTimer;
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyTypeObject PyScopeTimerType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.ScopeTimer",          /* tp_name */
		sizeof(PyScopeTimer),                 /* tp_basicsize */
		0,                                    /* tp_itemsize */
		(destructor)PyScopeTimer_dealloc,     /* tp_dealloc */
		0,                                    /* tp_print */
		0,                                    /* tp_getattr */
		0,                                    /* tp_setattr */
		0,                                    /* tp_reserved */
		0,                                    /* tp_repr */
		0,                                    /* tp_as_number */
		0,                                    /* tp_as_sequence */
		0,                                    /* tp_as_mapping */
		0,                                    /* tp_hash  */
		0,                                    /* tp_call */
		0,                                    /* tp_str */
		0,                                    /* tp_getattro */
		0,                                    /* tp_setattro */
		0,                                    /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /* tp_flags */
		"ScopeTimer objects",                 /* tp_doc */
		0,                                    /* tp_traverse */
		0,                                    /* tp_clear */
		0,                                    /* tp_richcompare */
		0,                                    /* tp_weaklistoffset */
		0,                                    /* tp_iter */
		0,                                    /* tp_iternext */
		0,                                    /* tp_methods */
		0,                                    /* tp_members */
		0,                                    /* tp_getset */
		0,                                    /* tp_base */
		0,                                    /* tp_dict */
		0,                                    /* tp_descr_get */
		0,                                    /* tp_descr_set */
		0,                                    /* tp_dictoffset */
		(initproc)PyScopeTimer_init,          /* tp_init */
		0,                                    /* tp_alloc */
		PyType_GenericNew,                    /* tp_new */
	};

	int Register(PyObject* module)
	{
		if (PyType_Ready(&PyScopeTimerType) < 0) return -1;

		Py_INCREF(&PyScopeTimerType);
		if (PyModule_AddObject(module, "ScopeTimer", (PyObject *)&PyScopeTimerType) < 0)
		{
			Py_DECREF(&PyScopeTimerType);
			return -1;
    	}
	}
} // namespace SRC::auto_graph