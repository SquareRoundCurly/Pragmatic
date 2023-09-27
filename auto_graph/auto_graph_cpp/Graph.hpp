#pragma once

// Standard library
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>

// auto_graph
#include "Node.hpp"
#include "Edge.hpp"

// Forward declarations
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
		struct TupleHash
		{
		size_t operator()(const std::tuple<std::string, std::string>& key) const
		{
			auto h1 = std::hash<std::string>{}(std::get<0>(key));
			auto h2 = std::hash<std::string>{}(std::get<1>(key));
			return h1 ^ h2;
		}
	};

	class Graph
	{
		private: // Fields
		int verticeCount = 0;
		std::unordered_map<Node, std::vector<Edge>> adjacency;

		public:
		std::unordered_map<std::string, PyObject*> pyNodes;
		std::unordered_map<std::tuple<std::string, std::string>, PyObject*, TupleHash> pyEdges;

		public: // Nodes
		const Node& GetNode(const std::string& name);
		Node AddNode(std::string name, PythonTask task = std::monostate());
		Node GetOrCreateNode(const std::string& name);

		public: // Edges
		const Edge& GetEdge(const std::string source, const std::string& target);
		Edge AddEdge(const Node& source, const Node& target, const Edge& edge);
		
		public: // Sort
		void TopologicalSort();
		std::vector<std::vector<Node>> GetGenerations();
		void PrintGenerations();

		private: // Internal functions
		void TopologicalSortUtil(const Node& source, std::unordered_map<Node, bool>& visited, std::stack<Node>& stack);
	};

	int Register_PyGraph(PyObject* module);
} // namesapce SRC::auto_graph