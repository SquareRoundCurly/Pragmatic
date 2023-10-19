#pragma once

// Standard library
#include <string>

namespace SRC::auto_graph
{
	void RaiseError(const char* message);
	inline void RaiseError(const std::string message) { RaiseError(message.c_str()); }
} // namespace SRC::auto_graph