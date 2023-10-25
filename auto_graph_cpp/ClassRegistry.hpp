#pragma once

// External
#include <pytypedefs.h>

// auto_graph
#include "MacroUtils.hpp"

namespace Pragmatic::auto_graph
{
	class Registry
	{
		public:
    	typedef int (*RegisterFunc)(PyObject*);

		public:
    	static void Register(RegisterFunc func);
    	static void CallAll(PyObject* module);
	};
} // namespace Pragmatic::auto_graph

#define REGISTER_CLASS_DETAIL(name, func) \
    static struct name                    \
    {                                     \
        name()                            \
        {                                 \
            Registry::Register(func);     \
        }                                 \
    } CONCATENATE(name, _instance);

#define REGISTER_CLASS(func) REGISTER_CLASS_DETAIL(UNIQUE_NAME(Registrar_), func)