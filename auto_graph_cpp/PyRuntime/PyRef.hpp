#pragma once

// External
#include "Python.h"

namespace Pragmatic::auto_graph
{
	class PyRef
	{
		private: // State
		PyObject* obj;

		public: // Constructors
		PyRef();                          // Default
		PyRef(PyObject* _obj);            // Wrapper
		PyRef(PyRef&& other);             // Move constructor
		PyRef& operator=(PyRef&& other);  // Move assignment
		PyRef& operator=(PyObject* _obj); // Conversion constructor
		~PyRef();                         // Destructor

		public: // Disable copying to prevent unintentional reference count bugs
		PyRef(const PyRef&) = delete;
		PyRef& operator=(const PyRef&) = delete;

		public: // Getters
		inline PyObject* get() const        { return obj; }
		inline PyObject* operator->() const { return obj; }
		inline PyObject*& operator*()       { return obj; }
		inline operator PyObject*() const   { return obj; } // Implicit cast to PyObject*

		public: // Utility
		inline bool is_null() const { return obj == nullptr; }
		void reset(PyObject* newObj = nullptr);
	};
} // namespace Pragmatic::atuo_graph
