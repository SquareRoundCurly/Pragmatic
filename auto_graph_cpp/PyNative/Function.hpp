#pragma once

// External
#include "pytypedefs.h"

// auto_graph
#include "../PyRuntime/PyRef.hpp"

namespace Pragmatic::auto_graph
{
	class Function
	{
		public: // Constructor
		explicit Function(PyObject* callable);
		explicit Function(PyRef&& callable);

		public: // Deleted constructors
		Function(const PyRef&)               = delete;
		Function& operator=(const Function&) = delete;

		public: // Operators	
		PyRef operator()(PyObject* arg);

		private:
		PyRef callable;
	};
} // namespace Pragmatic::auto_graph
