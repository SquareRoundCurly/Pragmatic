// Source header
#include "ClassRegistry.hpp"

// Standard library
#include <vector>

namespace Pragmatic::auto_graph
{
	static std::vector<Registry::RegisterFunc> registrationFunctions;

void Registry::Register(RegisterFunc func)
{
    registrationFunctions.push_back(func);
}

void Registry::CallAll(PyObject* module)
{
    for (const auto& func : registrationFunctions)
	{
        func(module);
    }
}
} // Pragmatic::auto_graph