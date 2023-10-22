#pragma once

// External
#include <pytypedefs.h>

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
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
} // namespace Pragmatic::auto_graph