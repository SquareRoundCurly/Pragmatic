// Source header
#include "Graph.hpp"

// Standard library
#include <queue>
#include <set>

// auto_graph
#include "Out.hpp"
#include "Instrumentation.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "Task.hpp"

namespace Pragmatic::auto_graph
{
	Graph::Graph()
	{
		PROFILE_FUNCTION();
	}

	Graph::~Graph()
	{
		PROFILE_FUNCTION();
	}

	int Graph::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		PROFILE_FUNCTION();

		return 0;
	}

	void Graph::PyClassDestruct(PyClass* self)
	{
		PROFILE_FUNCTION();
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

	Node* Graph::AddNode(const std::string& name, PyObject* task)
	{
        nodes.emplace_back(name, task);
        return &nodes.back();
    }

    void Graph::AddEdge(const std::string& fromName, const std::string& toName, PyObject* task)
	{
        Node* fromNode = GetNodeByName(fromName);
        Node* toNode = GetNodeByName(toName);

        if (fromNode && toNode)
		{
            edges.emplace_back(fromNode, toNode, task);
        }
    }

	std::vector<std::vector<Node*>> Graph::TopologicalSort()
	{
		std::vector<std::vector<Node*>> generations;
		std::queue<Node*> nodesQueue;

		// Initialize nodes with no incoming edges
		for (Node& node : nodes)
		{
			bool hasIncomingEdges = std::any_of(edges.begin(), edges.end(),
				[&node](const Edge& edge) { return edge.to == &node; });

			if (!hasIncomingEdges)
				nodesQueue.push(&node);
		}

		while (!nodesQueue.empty())
		{
			int currentGeneration = generations.size();
			generations.push_back(std::vector<Node*>());

			int currentGenerationSize = nodesQueue.size();
			for (int i = 0; i < currentGenerationSize; ++i)
			{
				Node* currentNode = nodesQueue.front();
				nodesQueue.pop();

				currentNode->generation = currentGeneration;
				generations[currentGeneration].push_back(currentNode);

				// Find all nodes dependent on the current node
				std::vector<Node*> dependentNodes;
				for (const Edge& edge : edges)
				{
					if (edge.from == currentNode)
					{
						Node* toNode = edge.to;
						dependentNodes.push_back(toNode);
					}
				}

				// Check if dependent nodes can be processed
				for (Node* dependentNode : dependentNodes)
				{
					bool allIncomingProcessed = std::all_of(edges.begin(), edges.end(),
						[dependentNode](const Edge& inEdge)
						{
							return inEdge.to == dependentNode || dependentNode->generation != -1;
						});

					if (allIncomingProcessed)
					{
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
		PyObject* task = nullptr;  // Optional callable object

		if (!PyArg_ParseTuple(args, "s|O", &name_cstr, &task))  // Use "s|O" to make callable optional
		{
			ThrowPythonError("Expected a str name");
		}

		auto name = std::string(name_cstr);

		AddNode(name, task);

		Py_RETURN_NONE;
	}

	PyObject *Graph::AddEdge(PyObject *self, PyObject *args)
	{
		PROFILE_FUNCTION();

		const char* fromNameCStr;
		const char* toNameCStr;
		PyObject* task = nullptr;

		if (!PyArg_ParseTuple(args, "ss|O", &fromNameCStr, &toNameCStr, &task))
		{
			ThrowPythonError("Expected a str name");
		}

		auto fromName = std::string(fromNameCStr);
		auto toName = std::string(toNameCStr);

		AddEdge(fromName, toName, task);

		Py_RETURN_NONE;
	}

	PyObject *Graph::TopologicalSort(PyObject *self, PyObject *args)
	{
		auto generations = TopologicalSort();

		PyObject* result_list = PyList_New(0);
		int counter = 0;

		for (auto& generation : generations)
		{
			PyObject* generation_list = PyList_New(0);

			for (auto& node : generation)
			{
				PyObject* node_name = PyUnicode_DecodeUTF8(node->name.c_str(), node->name.size(), "strict");
				PyList_Append(generation_list, node_name);
				Py_DECREF(node_name);
			}

			PyList_Append(result_list, generation_list);
			Py_DECREF(generation_list);

			counter++;
		}

		return result_list;
	}

	PyObject* Graph::RunTasks(PyObject* self, PyObject* args)
	{
		Out() << "Running tasks" << std::endl;

		auto generations = TopologicalSort();

		std::set<Edge*> processedEdges;

		for (auto& generation : generations)
		{
			for (auto& node : generation)
			{
				if (node->task)
				{
					PyObject* node_name = PyUnicode_DecodeUTF8(node->name.c_str(), node->name.size(), "strict");

					PyObject* args_tuple = PyTuple_Pack(1, node_name);

					auto& task = ConvertPyObjectToTask(node->task);
					task.Exec(NULL, args_tuple, NULL);
				}

				// Process outgoing edges for this node
				for (Edge& edge : edges)
				{
					if (edge.from == node && processedEdges.find(&edge) == processedEdges.end())
					{
						// Execute the edge's task
						if (edge.task)
						{
							PyObject* from_node_name = PyUnicode_DecodeUTF8(edge.from->name.c_str(), edge.from->name.size(), "strict");
							PyObject* to_node_name = PyUnicode_DecodeUTF8(edge.to->name.c_str(), edge.from->name.size(), "strict");
							PyObject* args_tuple = PyTuple_Pack(2, from_node_name, to_node_name);

							auto& edgeTask = ConvertPyObjectToTask(edge.task);
							edgeTask.Exec(NULL, args_tuple, NULL);  // You can pass appropriate arguments here
						}
						processedEdges.insert(&edge);
					}
				}
			}
		}

		Py_RETURN_NONE;
	}
}
