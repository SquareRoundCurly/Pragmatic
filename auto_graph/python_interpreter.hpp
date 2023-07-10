#pragma once

// Standard library
#include <vector>

// External
#define PY_SSIZE_T_CLEAN
#include <Python.h>

namespace SRC::AG
{
	class PythonInterpreter
	{
		public:
		PythonInterpreter();
		~PythonInterpreter();
		void CreateSubinterpreter();
		void Run(int index);

		private:
		PyThreadState* mainstate;
		std::vector<PyThreadState*> substates;
	};
	
} // namespace SRC::AG