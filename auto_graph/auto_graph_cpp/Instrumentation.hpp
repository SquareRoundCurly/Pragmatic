#pragma once

// Standard library
#include <string>

// Forward declarations
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
	class ScopeTimer
	{
		public:
		 ScopeTimer(const std::string& name);
		~ScopeTimer();

		private:
		const std::string name;
	};

	int Register(PyObject* module);
} // namespace SRC::auto_graph