// Source header
#include "Graph.hpp"

// External
#include "Python.h"

// auto_graph
#include "Node.hpp"

namespace SRC::auto_graph
{
	void Graph::AddEdge(const Node& source, const Node& target, const Edge& edge)
	{
		adjacency[source].push_back(edge);
		if (adjacency.find(target) == adjacency.end())
			adjacency[target] = { };
	}

	void Graph::TopologicalSort()
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

	std::vector<std::vector<Node>> Graph::GetGenerations()
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

	void Graph::PrintGenerations()
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

	void Graph::TopologicalSortUtil(const Node& source, std::unordered_map<Node, bool>& visited, std::stack<Node>& stack)
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

	#pragma region Python

	typedef struct PyGraph
	{
		PyObject_HEAD;
		Graph* graph;
	};

	static int PyGraph_init(PyGraph *self, PyObject *args, PyObject *kwds)
	{
		self->graph = new Graph();

		return 0;
	}

	static void PyGraph_dealloc(PyGraph* self)
	{
		delete self->graph;
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyObject* PyGraph_add_edge(PyGraph* self, PyObject* args)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Graph object not initialized");
			return nullptr;
		}


		// Parse two string arguments
		char *source, *target;
		if (!PyArg_ParseTuple(args, "ss", &source, &target))
		{
			return nullptr;
		}

		Node A { std::string(source) };
    	Node B { std::string(target) };

		self->graph->AddEdge(A, B, Edge{ A, B });

		Py_RETURN_NONE;
	}

	static PyObject* PyGraph_print_topological_generations(PyGraph* self, PyObject* args)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Graph object not initialized");
			return nullptr;
		}

		self->graph->PrintGenerations();

		Py_RETURN_NONE;
	}

	static PyObject* PyGraph_get_node_generations(PyGraph* self, PyObject* args)
	{
		if (!self->graph)
		{
			PyErr_SetString(PyExc_RuntimeError, "Graph object not initialized");
			return nullptr;
		}

		std::vector<std::vector<Node>> generations = self->graph->GetGenerations();
		
		// Top level list
		PyObject* pyGenerations = PyList_New(generations.size());
		if (!pyGenerations) return nullptr;

		// Internal lists
		for (size_t i = 0; i < generations.size(); i++)
		{
			PyObject* pyGeneration = PyList_New(generations[i].size());
			if (!pyGeneration)
			{
				Py_DECREF(pyGenerations);
				return nullptr;
			}

			for (size_t j = 0; j < generations[i].size(); j++)
			{
				auto* nodeObj = CreatePyNode(generations[i][j]);

				PyList_SetItem(pyGeneration, j, nodeObj);
			}

			PyList_SetItem(pyGenerations, i, pyGeneration);
		}

		return pyGenerations;
	}

	static PyMethodDef PyGraph_methods[] =
	{
		{ "add_edge", (PyCFunction)PyGraph_add_edge, METH_VARARGS, "Adds a new edge to the graph, from source node to target node" },
		{ "print_topological_generations", (PyCFunction)PyGraph_print_topological_generations, METH_NOARGS, "Sorts the graph into topological generations & prints them" },
		{ "get_node_generations", (PyCFunction)PyGraph_get_node_generations, METH_NOARGS, "Sorts the graph into topological generations" },
		{ nullptr }  // sentinel
	};

	static PyTypeObject PyGraphType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.Graph",                   /* tp_name */
		sizeof(PyGraph),                          /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyGraph_dealloc,              /* tp_dealloc */
		0,                                        /* tp_print */
		0,                                        /* tp_getattr */
		0,                                        /* tp_setattr */
		0,                                        /* tp_reserved */
		0,                                        /* tp_repr */
		0,                                        /* tp_as_number */
		0,                                        /* tp_as_sequence */
		0,                                        /* tp_as_mapping */
		0,                                        /* tp_hash  */
		0,                                        /* tp_call */
		0,                                        /* tp_str */
		0,                                        /* tp_getattro */
		0,                                        /* tp_setattro */
		0,                                        /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
		"Graph objects",                          /* tp_doc */
		0,                                        /* tp_traverse */
		0,                                        /* tp_clear */
		0,                                        /* tp_richcompare */
		0,                                        /* tp_weaklistoffset */
		0,                                        /* tp_iter */
		0,                                        /* tp_iternext */
		PyGraph_methods,                          /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		0,                                        /* tp_dictoffset */
		(initproc)PyGraph_init,                   /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	int Register_PyGraph(PyObject* module)
	{
		if (PyType_Ready(&PyGraphType) < 0) return -1;

		Py_INCREF(&PyGraphType);
		if (PyModule_AddObject(module, "Graph", (PyObject*)&PyGraphType) < 0)
		{
			Py_DECREF(&PyGraphType);
			return -1;
		}
	}

	#pragma endregion Python
} // namespace SRC::auto_graph