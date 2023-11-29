// Source header
#include "Graph.hpp"

// Standard library
#include <queue>

// auto_graph
#include "Out.hpp"
#include "Instrumentation.hpp"
#include "PyRuntime/PythonUtils.hpp"

namespace Pragmatic::auto_graph
{
	Graph::Graph()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph ctor" << std::endl;
	}

	Graph::~Graph()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph dtor" << std::endl;
	}

	int Graph::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph init" << std::endl;

		return 0;
	}

	void Graph::PyClassDestruct(PyClass* self)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] Graph deinit" << std::endl;
	}

	Node* Graph::GetNodeByName(const std::string& name)
	{
        auto it = std::find_if(nodes.begin(), nodes.end(), [&name](const Node& node) { return node.name == name; });

        if (it != nodes.end())
		{
            return &(*it);
        }

        return nullptr;
    }

	Node* Graph::AddNode(const std::string& name)
	{
        nodes.emplace_back(name);
        return &nodes.back();
    }

    void Graph::AddEdge(const std::string& fromName, const std::string& toName)
	{
        Node* fromNode = GetNodeByName(fromName);
        Node* toNode = GetNodeByName(toName);

        if (fromNode && toNode)
		{
            edges.emplace_back(fromNode, toNode);
        }
    }

	std::vector<std::vector<Node*>> Graph::TopologicalSort()
	{
		std::vector<std::vector<Node*>> generations;
		std::queue<Node*> nodesQueue;

		// Initialize nodes with no incoming edges
		for (Node& node : nodes) {
			bool hasIncomingEdges = std::any_of(edges.begin(), edges.end(),
				[&node](const Edge& edge) { return edge.to == &node; });

			if (!hasIncomingEdges) {
				nodesQueue.push(&node);
			}
		}

		while (!nodesQueue.empty()) {
			int currentGeneration = generations.size();
			generations.push_back(std::vector<Node*>());

			int currentGenerationSize = nodesQueue.size();
			for (int i = 0; i < currentGenerationSize; ++i) {
				Node* currentNode = nodesQueue.front();
				nodesQueue.pop();

				currentNode->generation = currentGeneration;
				generations[currentGeneration].push_back(currentNode);

				// Find all nodes dependent on the current node
				std::vector<Node*> dependentNodes;
				for (const Edge& edge : edges) {
					if (edge.from == currentNode) {
						Node* toNode = edge.to;
						dependentNodes.push_back(toNode);
					}
				}

				// Check if dependent nodes can be processed
				for (Node* dependentNode : dependentNodes) {
					bool allIncomingProcessed = std::all_of(edges.begin(), edges.end(),
						[dependentNode](const Edge& inEdge) {
							return inEdge.to == dependentNode || dependentNode->generation != -1;
						});

					if (allIncomingProcessed) {
						nodesQueue.push(dependentNode);
					}
				}
			}
		}

		return generations;
	}

	PyObject* Graph::AddNode(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		const char* name_cstr;
		if (!PyArg_ParseTuple(args, "s", &name_cstr))
		{
			ThrowPythonError("Expected a str name");
		}
		auto name = std::string(name_cstr);

		Out() << "[auto_graph] Added node: " << name << std::endl;
		AddNode(name);

		Py_RETURN_NONE;
	}

	PyObject *Graph::AddEdge(PyObject *self, PyObject *args)
	{
		PROFILE_FUNCTION();

		const char* fromNameCStr;
		const char* toNameCStr;
		if (!PyArg_ParseTuple(args, "ss", &fromNameCStr, &toNameCStr))
		{
			ThrowPythonError("Expected a str name");
		}
		auto fromName = std::string(fromNameCStr);
		auto toName = std::string(toNameCStr);

		Out() << "[auto_graph] Added edge: [" << fromNameCStr << ", " << toName << "]"<< std::endl;
		AddEdge(fromName, toName);

		Py_RETURN_NONE;
	}

	PyObject *Graph::TopologicalSort(PyObject *self, PyObject *args)
	{
		auto generations = TopologicalSort();
		int counter = 0;

		for (auto& generation : generations)
		{
			Out() << counter++ << ": ";
			for (auto& node : generation)
			{
				Out() << node->name << " ";
			}
			Out() << std::endl;
		}
	
		Py_RETURN_NONE;
	}
}
