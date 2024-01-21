// Source header
#include "Graph.hpp"

// Standard library
#include <queue>
#include <set>
#include <unordered_map>

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

		auto newEdge = Edge(fromNode, toNode, task);

		auto it = std::find_if(edges.begin(), edges.end(), [&newEdge](const Edge& edge)
		{
			return newEdge.from == edge.from && newEdge.to == edge.to;
		});

		if (it == edges.end() && fromNode && toNode)
		{
			edges.emplace_back(fromNode, toNode, task);
		}
	}

	std::vector<std::vector<Node*>> Graph::TopologicalSort()
	{
		std::unordered_map<Node*, int> incomingEdgesCount;
		std::queue<Node*> processQueue;
		std::vector<std::vector<Node*>> generations;

		// Initialize incoming edges count
		for (auto& node : nodes)
			incomingEdgesCount[&node] = 0;

		// Count incoming edges for each node
		for (auto& edge : edges)
			incomingEdgesCount[edge.to]++;

		// Enqueue nodes with no incoming edges
		for (auto& node : nodes)
			if (incomingEdgesCount[&node] == 0)
				processQueue.push(&node);

		// Process nodes generation by generation
		while (!processQueue.empty())
		{
			int generationSize = processQueue.size();
			std::vector<Node*> currentGeneration;

			for (int i = 0; i < generationSize; ++i)
			{
				Node* currentNode = processQueue.front();
				processQueue.pop();
				currentGeneration.push_back(currentNode);

				// Decrement the incoming edge count for each dependent node
				for (auto& edge : edges)
				{
					if (edge.from == currentNode)
					{
						Node* dependentNode = edge.to;
						incomingEdgesCount[dependentNode]--;
						if (incomingEdgesCount[dependentNode] == 0)
							processQueue.push(dependentNode);
					}
				}
			}

			generations.push_back(currentGeneration);
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
							PyObject* to_node_name = PyUnicode_DecodeUTF8(edge.to->name.c_str(), edge.to->name.size(), "strict");
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
