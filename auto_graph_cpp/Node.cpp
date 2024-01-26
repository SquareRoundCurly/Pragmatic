// Source header
#include "Node.hpp"

namespace Pragmatic::auto_graph
{
	Node::Node(const std::string& n, PyObject* task) : name(n), task(task)
	{

	}
} // namespace Pragmatic::auto_graph