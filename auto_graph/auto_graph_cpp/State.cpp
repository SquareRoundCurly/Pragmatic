// Source header
#include "State.hpp"

// External
#include "Python.h"

namespace SRC::auto_graph
{
	InterpreterState* InterpreterState::Get()
	{
		auto* module = AutoGraphState::Get()->GetModule();
		return (InterpreterState*)PyModule_GetState((PyObject*)module);
	}

	AutoGraphState * SRC::auto_graph::AutoGraphState::Get()
	{
		static AutoGraphState autoGraphState;
		return &autoGraphState;
	}

	PyObject* SRC::auto_graph::AutoGraphState::GetModule() const
	{
		return auto_graph_module;
	}

	void SRC::auto_graph::AutoGraphState::SetModule(PyObject* module)
	{
		auto_graph_module = module;
	}
} // namespace SRC::auto_graph