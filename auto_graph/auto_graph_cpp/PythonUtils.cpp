// Source header
#include "PythonUtils.hpp"

// External
#include <Python.h>

namespace SRC::auto_graph
{
	void RaiseError(const char* message)
	{
		PyGILState_STATE gstate;
		gstate = PyGILState_Ensure();

		PyErr_SetString(PyExc_ValueError, message);

		PyGILState_Release(gstate);
	}
}
