// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>

// External
#include <Python.h>

// auto_graph
#include "instrument.hpp"

namespace SRC::auto_graph
{
	void Test(const std::string& str)
	{
		Subinterpreter interpreter;
		interpreter.Run(str);
	}

	Subinterpreter::Subinterpreter()
	{
		PROFILE_FUNCTION();

		mainThreadState = PyThreadState_Get();

		// Create a new sub-interpreter
		const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
		Py_NewInterpreterFromConfig(&subinterpreterThreadState, &config);

		thread = std::thread([this]()
		{
			auto newState = PyThreadState_New(subinterpreterThreadState->interp);

			while (true)
			{
				std::string code;
				queue.wait_dequeue(code);

				if (code == "__END__") break;

				{
					PROFILE_SCOPE("PyRun_SimpleString");

					PyEval_RestoreThread(newState);
					auto* oldState = PyThreadState_Swap(newState);
					PyRun_SimpleString(code.c_str());
					PyThreadState_Swap(oldState);
				}
			}
		
			PyThreadState_Clear(newState);
			PyThreadState_DeleteCurrent();
			
			std::cout << "Exiting thread: " << std::this_thread::get_id() << std::endl;
		});

		PyThreadState_Swap(mainThreadState);
	}

	Subinterpreter::~Subinterpreter()
	{
		PROFILE_FUNCTION();

		queue.enqueue("__END__");
		thread.join();

		PyThreadState_Swap(subinterpreterThreadState);
		Py_EndInterpreter(subinterpreterThreadState);
		PyThreadState_Swap(mainThreadState);
	}

	void Subinterpreter::Run(const std::string& code)
	{
		PROFILE_FUNCTION();

		queue.enqueue(code);
	}
} // namespace SRC::auto_graph
