#pragma once

// Standard library
#include <string>

namespace Pragmatic::auto_graph
{
	struct Node
	{
		std::string name;
		int generation = 0;
		// Other data fields specific to the node

		Node(const std::string& n) : name(n) { }
	};
} // namespace Pragmatic::auto_graph