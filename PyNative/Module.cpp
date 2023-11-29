// Source header
#include "Module.hpp"

// External
#include "Python.h"

// auto_graph
#include "PyRuntime/PythonUtils.hpp"

namespace Pragmatic::auto_graph
{
	Module::Module(const std::string& name) : name(name)
	{
		module = PyImport_ImportModule(name.c_str());

		if (!module)
			ThrowPythonError("Couldn't imort module: " + name);
	}
	
	Function Module::operator[](const std::string& key)
	{
		PyRef attribute = PyObject_GetAttrString(module, key.c_str());
		if (!attribute)
			ThrowPythonError("Couldn't load attribute: " + key);

		return Function(std::move(attribute));
	}
}