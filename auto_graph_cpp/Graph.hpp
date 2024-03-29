#pragma once

// Standard library
#include <vector>
#include <string>
#include <list>

// auto_graph
#include "Node.hpp"
#include "Edge.hpp"
#include "PyRuntime/PyClass.hpp"

namespace Pragmatic::auto_graph
{
	class Graph : public PyClass
	{
		public: // Constructors
		Graph();
		~Graph();

		public: // Python
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) override;
		virtual void PyClassDestruct(PyClass* self) override;

		private: // Graph
		Node* GetNodeByName(const std::string& name);
		Node* AddNode(const std::string& name, PyObject* task = nullptr);
		void AddEdge(const std::string& fromName, const std::string& toName, PyObject* task);
		std::vector<Node*> GetParentNodes(const std::string& nodeName);
		std::vector<std::vector<Node*>> TopologicalSort();

		public:
		PyObject* AddNode(PyObject* self, PyObject* args);
		PyObject* AddEdge(PyObject* self, PyObject* args);
		PyObject* TopologicalSort(PyObject* self, PyObject* args);
		PyObject* GetNode(PyObject* self, PyObject* args);
		PyObject* GetParentNodes(PyObject* self, PyObject* args);

		public: // Experimental
		PyObject* RunTasks(PyObject* self, PyObject* args);

		private:
		std::list<Node> nodes;
    	std::vector<Edge> edges;
		std::vector<std::vector<Node*>> generations;
	};
} // namespace Pragmatic::auto_graph