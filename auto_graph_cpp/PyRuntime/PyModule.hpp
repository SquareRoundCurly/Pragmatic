#pragma once

// Standard library
#include <stdexcept>

// External
#include <pytypedefs.h>

// auto_graph
#include "ClassRegistry.hpp"
#include "../Instrumentation.hpp"

// Forward declarations
typedef int (*visitproc)(PyObject *, void *);

namespace Pragmatic::auto_graph
{
	template <class ModuleClass>
	ModuleClass* GetModule(PyObject* module = nullptr);

	template <class ModuleClass>
	int init(PyObject *module)
	{
		PROFILE_BEGIN_SESSION("auto_graph.json");

		// Placement new, call constructor
		ModuleClass* moduleState = (ModuleClass*) PyModule_GetState(module);
		new (moduleState) ModuleClass();

		// Store Python module object
		auto moduleClassInstance = GetModule<ModuleClass>(module);

		// Initalize class types
		int returnCode = Registry::RegisterTypes(module);
		if (returnCode != 0) return returnCode;

		return moduleClassInstance->init(module);
	}

	template <class ModuleClass>
	int traverse(PyObject* module, visitproc visit, void* arg)
	{
		return GetModule<ModuleClass>(module)->traverse(module, visit, arg);
	}

	template <class ModuleClass>
	int clear(PyObject* module)
	{
		return GetModule<ModuleClass>()->clear(module);
	}

	template <class ModuleClass>
	void free(void* module)
	{
		GetModule<ModuleClass>()->free(module);
	
		PROFILE_END_SESSION();
	}

	template <class ModuleClass, PyObject* (ModuleClass::*moduleMethod)(PyObject*, PyObject*)>
	PyObject* method(PyObject* self, PyObject* args)
	{
		ModuleClass* moduleInstance = GetModule<ModuleClass>();
		return (moduleInstance->*moduleMethod)(self, args);
	}
} // namespace Pragmatic::auto_graph