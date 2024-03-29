#pragma once

// Standard library
#include <vector>

// External
#include <pytypedefs.h>

// auto_graph
#include "MacroUtils.hpp"

namespace Pragmatic::auto_graph
{
	struct PyClassType
	{
		PyTypeObject& type;
		const char* name;
	};

	class Registry
	{
		public:
		typedef int (*RegisterFunc)(PyObject*);

		public:
		static std::vector<PyClassType>& GetTypes();
		static void Register(PyTypeObject& type, const char* name);
		static int RegisterTypes(PyObject* module);
	};
} // namespace Pragmatic::auto_graph

#define REGISTER_CLASS_DETAIL(unique_name, func, name) \
    static struct unique_name                          \
    {                                                  \
        unique_name()                                  \
        {                                              \
            Registry::Register(func, name);            \
        }                                              \
    } CONCATENATE(unique_name, _instance);

#define REGISTER_CLASS(func, name) REGISTER_CLASS_DETAIL(UNIQUE_NAME(CONCATENATE(CONCATENATE(Registrar_, name), _)), func, #name)