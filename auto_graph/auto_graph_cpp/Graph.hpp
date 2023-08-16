#pragma once

// Standard library
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>

#include "Node.hpp"
#include "Edge.hpp"

namespace SRC::auto_graph
{
	class Graph
	{
		private: // Fields
		int verticeCount = 0;
		std::unordered_map<Node, std::vector<Edge>> adjacency;

		public:
		void AddEdge(const Node& source, const Node& target, const Edge& edge);
		void TopologicalSort();
		std::vector<std::vector<Node>> GetGenerations();
		void PrintGenerations();

		private: // Internal functions
		void TopologicalSortUtil(const Node& source, std::unordered_map<Node, bool>& visited, std::stack<Node>& stack);
	};

	int Register_PyGraph(PyObject* module);
} // namesapce SRC::auto_graph