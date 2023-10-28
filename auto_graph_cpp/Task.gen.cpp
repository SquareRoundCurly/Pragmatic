// Source header
#include "Task.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRuntime/PyClass.hpp"
#include "PyRuntime/ClassRegistry.hpp"
#include "Out.hpp"

static PyObject* test(PyObject*, PyObject*, PyObject*)
{
	return NULL;
}

namespace Pragmatic::auto_graph
{
	using PyTask = PyClassWrapper<Task>;

	static PyMethodDef PyTask_methods[] =
	{
		{ "exec", (PyCFunction)PyTask::MethodKw<&Task::Exec>, METH_VARARGS | METH_KEYWORDS, "Executes the stored callable." },
		{ nullptr }  // sentinel
	};

	static PyTypeObject PyType_Pragmatic_auto_graph_Task =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Task",                    /* tp_name */
		sizeof(PyTask),                           /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyTask::PyClassDestruct,      /* tp_dealloc */
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
		PyTask_methods,                           /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		0,                                        /* tp_dictoffset */
		(initproc)PyTask::PyClassInit,            /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	REGISTER_CLASS(PyType_Pragmatic_auto_graph_Task, "Task");
} // namespace Pragmatic::auto_graph