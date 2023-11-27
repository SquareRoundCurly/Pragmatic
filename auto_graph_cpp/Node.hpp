#pragma once

// Standard library
#include <string>

namespace Pragmatic::auto_graph
{
	struct Node
	{
		std::string name;
		int data;

		// Constructor
		Node(const std::string& n) : name(n) { }
	};
} // namespace Pragmatic::auto_graph