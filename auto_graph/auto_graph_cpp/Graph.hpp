#pragma once

// Standard library
#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <queue>

namespace SRC::auto_graph
{
	template <typename Node, typename Edge>
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

	template <typename Node, typename Edge>
	void Graph<Node, Edge>::AddEdge(const Node& source, const Node& target, const Edge& edge)
	{
		adjacency[source].push_back(edge);
		if (adjacency.find(target) == adjacency.end())
			adjacency[target] = { };
	}

	template <typename Node, typename Edge>
	void Graph<Node, Edge>::TopologicalSortUtil(const Node& source, std::unordered_map<Node, bool>& visited, std::stack<Node>& stack)
	{
		visited[source] = true;

		for (const Edge& edge : adjacency[source])
		{
			Node target = edge.to; // Assuming the Edge type has a member 'to' pointing to its end node
			if (!visited[target])
				TopologicalSortUtil(target, visited, stack);
		}

		stack.push(source);
	}

	template <typename Node, typename Edge>
	void Graph<Node, Edge>::TopologicalSort()
	{
		std::stack<Node> stack;
		std::unordered_map<Node, bool> visited;

		for (const auto &pair : adjacency)
			visited[pair.first] = false;

		for (const auto &pair : adjacency)
			if (!visited[pair.first])
				TopologicalSortUtil(pair.first, visited, stack);

		while (!stack.empty())
		{
			std::cout << stack.top() << " " << std::endl; // Assuming the Node type has an overloaded << operator
			stack.pop();
		}
	}

	template <typename Node, typename Edge>
	std::vector<std::vector<Node>> Graph<Node, Edge>::GetGenerations()
	{
		std::vector<std::vector<Node>> generations;

		std::unordered_map<Node, int> indegree;
		for (const auto &pair : adjacency)
			for (const Edge &edge : pair.second)
				indegree[edge.to]++;

		std::queue<Node> queue;
		for (const auto &pair : adjacency)
			if (indegree[pair.first] == 0)
				queue.push(pair.first);

		while (!queue.empty())
		{
			int size = queue.size();
			std::vector<Node> currentGeneration;

			for (int i = 0; i < size; i++)
			{
				Node curr = queue.front();
				queue.pop();

				currentGeneration.push_back(curr);

				for (const Edge &edge : adjacency[curr])
				{
					indegree[edge.to]--;
					if (indegree[edge.to] == 0)
						queue.push(edge.to);
				}
			}

			generations.push_back(currentGeneration);
		}

		return generations;
	}

	template <typename Node, typename Edge>
	void Graph<Node, Edge>::PrintGenerations()
	{
		std::vector<std::vector<Node>> gens = GetGenerations();
		
		int generationNumber = 1;  // Starts from 1 for the first generation.
		for (const auto& generation : gens)
		{
			std::cout << "Generation " << generationNumber << ": ";
			for (const auto& node : generation)
			{
				std::cout << node << " ";  // Assuming Node type has overloaded << operator
			}
			std::cout << "\n";
			generationNumber++;
		}
	}
} // namesapce SRC::auto_graph


// Example Node and Edge classes
struct MyNode
{
    std::string id;
    // Potentially other member variables and functions

    bool operator==(const MyNode &other) const
	{
        return id == other.id;
    }

    friend std::ostream& operator<<(std::ostream &os, const MyNode &node)
	{
        os << node.id;
        return os;
    }
};

struct MyEdge
{
    MyNode from;
    MyNode to;
    // Potentially other member variables and functions
};

namespace std {
    template <>
    struct hash<MyNode> {
        size_t operator()(const MyNode &node) const {
            return hash<std::string>()(node.id);
        }
    };
}

void Test()
{
	using namespace SRC::auto_graph;

	Graph<MyNode, MyEdge> g;
    
    MyNode A { "A" };
    MyNode B { "B" };
    MyNode C { "C" };
    MyNode D { "D" };
    MyNode E { "E" };
    MyNode F { "F" };
    MyNode G { "G" };
    MyNode H { "H" };
    MyNode I { "I" };
    MyNode J { "J" };

    g.AddEdge(A, B, MyEdge { A, B });
    g.AddEdge(A, C, MyEdge { A, C });
    g.AddEdge(B, D, MyEdge { B, D });
    g.AddEdge(C, E, MyEdge { C, E });
    g.AddEdge(D, F, MyEdge { D, F });
    g.AddEdge(E, F, MyEdge { E, F });
    g.AddEdge(F, G, MyEdge { F, G });
    g.AddEdge(F, H, MyEdge { F, H });
    g.AddEdge(C, I, MyEdge { C, I });
    g.AddEdge(I, J, MyEdge { I, J });

    std::cout << "Topological Sort of the given graph:\n";
    g.TopologicalSort();

	g.PrintGenerations();

    return;
}