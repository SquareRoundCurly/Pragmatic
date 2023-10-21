// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "Out.hpp"
#include "PythonUtils.hpp"

namespace Pragmatic
{
	template<typename T>
	struct method_traits;

	template<class C, typename R, typename... Args>
	struct method_traits<R(C::*)(Args...) const>
	{
		using class_type = C;
		using func_type = R(C::*)(Args...);
	};

	template<class C, typename R, typename... Args>
	struct method_traits<R(C::*)(Args...)>
	{
		using class_type = C;
		using func_type = R(C::*)(Args...);
	};
} // namespace Pragmatic


namespace Pragmatic::PythonUtils
{
	using namespace Pragmatic::auto_graph;

	template <class ModuleClass>
	ModuleClass* GetModule()
	{
		if (auto_graph_cpp::module == nullptr)
		{
			PyModuleDef* moduleDef = &ModuleClass::moduleDef;
			ModuleClass::module = PyState_FindModule(moduleDef);
		}
		return reinterpret_cast<ModuleClass*>(PyModule_GetState(ModuleClass::module));
	}

	template <class ModuleClass, typename T>
	ModuleClass* GetModule(T* module)
	{
		if (auto_graph_cpp::module == nullptr) ModuleClass::module = (PyObject*)module;
		return reinterpret_cast<ModuleClass*>(PyModule_GetState((PyObject*)module));
	}

	template <class ModuleClass>
	int traverse(PyObject* module, visitproc visit, void* arg)
	{
		return GetModule<ModuleClass>(module)->traverse(module, visit, arg);
	}

	template <class ModuleClass>
	int clear(PyObject* module)
	{
		return GetModule<ModuleClass>(module)->clear(module);
	}

	template <class ModuleClass>
	void free(void* module)
	{
		GetModule<ModuleClass>(module)->free(module);
	}

	template <class ModuleClass>
	int init(PyObject *module)
	{
		return GetModule<ModuleClass>(module)->init(module);
	}

	template <class ModuleClass, PyObject* (ModuleClass::*moduleMethod)(PyObject*, PyObject*)>
	PyObject* method(PyObject* self, PyObject* args)
	{
		ModuleClass* moduleInstance = GetModule<ModuleClass>();
		return (moduleInstance->*moduleMethod)(self, args);
	}
}

namespace Pragmatic::auto_graph
{
	static PyMethodDef methods[] =
	{
		{ "cleanup", Pragmatic::PythonUtils::method<auto_graph_cpp, &auto_graph_cpp::cleanup>,  METH_NOARGS,  "Cleanup function to be called before exit"                      },
		{ "reinit",  Pragmatic::PythonUtils::method<auto_graph_cpp, &auto_graph_cpp::reinit>,   METH_NOARGS,  "Callback for module reloads"                                    },
		{ "print",   Pragmatic::PythonUtils::method<auto_graph_cpp, &auto_graph_cpp::print>,    METH_VARARGS, "A thread safe print function"                                   },
		{ "task",    Pragmatic::PythonUtils::method<auto_graph_cpp, &auto_graph_cpp::add_task>, METH_VARARGS, "Python files, strings or functions to add to the executor pool" },
		{ NULL,      NULL,                                                                      0,            NULL                                                             }
	};

	static PyModuleDef_Slot slots[] =
	{
		{ Py_mod_exec, (void*)Pragmatic::PythonUtils::init<auto_graph_cpp> },
		{ Py_mod_multiple_interpreters, Py_MOD_PER_INTERPRETER_GIL_SUPPORTED },
		{ 0, NULL }
	};

	PyModuleDef auto_graph_cpp::moduleDef =
	{
		PyModuleDef_HEAD_INIT,
		"auto_graph_cpp",                                 // name of module 
		NULL,                                             // module documentation, may be NULL 
		sizeof(auto_graph_cpp),                           // size of per-interpreter state of the module
		methods,                                          // methods
		slots,                                            // slots
		Pragmatic::PythonUtils::traverse<auto_graph_cpp>, // traverse
		Pragmatic::PythonUtils::clear<auto_graph_cpp>,    // clear
		Pragmatic::PythonUtils::free<auto_graph_cpp>      // free
	};

	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "cleanup" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::reinit(PyObject* self, PyObject* args)
	{
		Out() << "[auto_graph] " << "reinit" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::print(PyObject* self, PyObject* args)
	{
		const char* str;
		if (!PyArg_ParseTuple(args, "s", &str)) // Get a string
		{
			PyErr_SetString(PyExc_TypeError, "parameter must be a string");
			return NULL;
		}

		Pragmatic::auto_graph::Out() << str << std::endl;

		Py_RETURN_NONE;

	}

	PyObject* auto_graph_cpp::add_task(PyObject* self, PyObject* args)
	{
		

		Py_RETURN_NONE;
	}

	int auto_graph_cpp::init(PyObject *module)
	{
		Out() << "[auto_graph] " << "init" << std::endl;

		return 0;
	}

	int auto_graph_cpp::traverse(PyObject* module, visitproc visit, void* arg)
	{
		Out() << "[auto_graph] " << "traverse" << std::endl;

		return 0;
	}

	int auto_graph_cpp::clear(PyObject* module)
	{
		Out() << "[auto_graph] " << "clear" << std::endl;

		return 0;
	}

	void auto_graph_cpp::free(void* module)
	{
		Out() << "[auto_graph] " << "free" << std::endl;

		return;
	}
} // namespace Pragmatic::auto_graph

PyMODINIT_FUNC PyInit_auto_graph_cpp(void)
{
	using namespace Pragmatic::auto_graph;

    return PyModuleDef_Init(&auto_graph_cpp::moduleDef);
}

PyMODINIT_FUNC PyInit_auto_graph_cpp_d(void)
{
    return PyInit_auto_graph_cpp();
}