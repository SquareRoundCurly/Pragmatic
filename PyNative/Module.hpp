#pragma once

// Standard library
#include <string>

// auto_graph
#include "PyRuntime/PyRef.hpp"
#include "Function.hpp"

namespace Pragmatic::auto_graph
{
	class Module
	{
		public: // Constructor
		Module(const std::string& name);

		public:
		Function operator[](const std::string& key);

		private:
		std::string name;
		PyRef module;
	};
} // namespace Pragmatic::auto_graph