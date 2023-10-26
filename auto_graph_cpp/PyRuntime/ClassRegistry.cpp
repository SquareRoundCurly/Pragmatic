// Source header
#include "ClassRegistry.hpp"

// Standard library
#include <vector>

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	struct PyClassType
	{
		PyTypeObject& type;
		const char* name;
	};
	static std::vector<PyClassType> types;

	void Registry::Register(PyTypeObject& type, const char* name)
	{
		types.push_back({type, name});
	}

	int Registry::RegisterTypes(PyObject* module)
	{
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