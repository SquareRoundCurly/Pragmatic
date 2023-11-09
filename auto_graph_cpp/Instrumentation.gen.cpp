// Source header
#include "Instrumentation.hpp"

// auto_graph
#include "PyRuntime/PyClass.hpp"
#include "PyRuntime/ClassRegistry.hpp"
#include "Out.hpp"

namespace Pragmatic::auto_graph
{
	using PyScopeTimer = PyClassWrapper<ScopeTimer>;

	static PyMethodDef PyTimer_methods[] =
	{
		{ nullptr } // sentinel
	};

	static PyTypeObject PyTimerType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.ScopeTimer",               /* tp_name */
		sizeof(PyScopeTimer),                      /* tp_basicsize */
		0,                                         /* tp_itemsize */
		(destructor)PyScopeTimer::PyClassDestruct, /* tp_dealloc */
		0,                                         /* tp_print */
		0,                                         /* tp_getattr */
		0,                                         /* tp_setattr */
		0,                                         /* tp_reserved */
		0,                                         /* tp_repr */
		0,                                         /* tp_as_number */
		0,                                         /* tp_as_sequence */
		0,                                         /* tp_as_mapping */
		0,                                         /* tp_hash  */
		0,                                         /* tp_call */
		0,                                         /* tp_str */
		0,                                         /* tp_getattro */
		0,                                         /* tp_setattro */
		0,                                         /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,  /* tp_flags */
		"ScopeTimer objects",                      /* tp_doc */
		0,                                         /* tp_traverse */
		0,                                         /* tp_clear */
		0,                                         /* tp_richcompare */
		0,                                         /* tp_weaklistoffset */
		0,                                         /* tp_iter */
		0,                                         /* tp_iternext */
		0,                                         /* tp_methods */
		0,                                         /* tp_members */
		0,                                         /* tp_getset */
		0,                                         /* tp_base */
		0,                                         /* tp_dict */
		0,                                         /* tp_descr_get */
		0,                                         /* tp_descr_set */
		0,                                         /* tp_dictoffset */
		(initproc)PyScopeTimer::PyClassInit,       /* tp_init */
		0,                                         /* tp_alloc */
		PyType_GenericNew,                         /* tp_new */
	};

	REGISTER_CLASS(PyTimerType, ScopeTimer);
} // namespace Pragmatic::auto_graph