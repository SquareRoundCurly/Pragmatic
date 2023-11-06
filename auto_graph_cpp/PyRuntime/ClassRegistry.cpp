// Source header
#include "ClassRegistry.hpp"

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	std::vector<PyClassType>& Registry::GetTypes()
	{
		static std::vector<PyClassType> types;
		return types;
	}

	void Registry::Register(PyTypeObject& type, const char* name)
	{
		auto& types = GetTypes();
		types.push_back({type, name});
	}

	int Registry::RegisterTypes(PyObject* module)
	{
		auto& types = GetTypes();
		for (const auto& type : types)
		{
			if (PyType_Ready(&type.type) < 0) return -1;

			Py_INCREF(&type.type);
			if (PyModule_AddObject(module, type.name, (PyObject*)&type.type) < 0)
			{
				Py_DECREF(&type.type);
				return -1;
			}
		}

		return 0;
	}
} // Pragmatic::auto_graph