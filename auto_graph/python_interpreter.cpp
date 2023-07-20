// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

// auto_graph
#include "instrument.hpp"
#include "streams.hpp"

namespace
{
	using namespace SRC::auto_graph;

	std::vector<Subinterpreter*> interpreters;
} // anonymous namespace


namespace SRC::auto_graph
{
	void Test(const std::string& code)
	{
		size_t lowest = LLONG_MAX;
		Subinterpreter* idleInterpreter = nullptr;

		for (auto& interpreter : interpreters)
		{
			if (interpreter->GetSize() <= lowest)
			{
				idleInterpreter = interpreter;
				lowest = interpreter->GetSize();
			}
		}

		idleInterpreter->Enque(code);
	}

	void Initialize()
	{
		auto concurrency = std::thread::hardware_concurrency();
		Out() << "Creating " << concurrency << " subinterpreters" << std::endl;
		for (size_t i = 0; i < concurrency; i++)
		{
			interpreters.push_back(new Subinterpreter);
		}
	}

	void Cleanup()
	{
		for (auto* interpreter : interpreters)
		{
			delete interpreter;
		}
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
			PyEval_RestoreThread(newState);

			while (true)
			{
				std::string code;
				queue.wait_dequeue(code);
				if (code == "__END__") break;

				{
					PROFILE_SCOPE("PyRun_SimpleString");
					auto* oldState = PyThreadState_Swap(newState);
					PyRun_SimpleString(code.c_str());
					PyThreadState_Swap(oldState);
				}
			}
		
			PyThreadState_Clear(newState);
			PyThreadState_DeleteCurrent();
			
			Out() << "Exiting thread: " << std::this_thread::get_id() << std::endl;
		});

		PyThreadState_Swap(mainThreadState);
	}

	Subinterpreter::~Subinterpreter()
	{
		PROFILE_FUNCTION();

		{
			PROFILE_SCOPE("Waiting for tasks to finish");
			queue.enqueue("__END__");
			thread.join();
		}

		{
			PROFILE_SCOPE("Finalizing subinterpreter");
			PyThreadState_Swap(subinterpreterThreadState);
			Py_EndInterpreter(subinterpreterThreadState);
			PyThreadState_Swap(mainThreadState);
		}
	}

	void Subinterpreter::Enque(const std::string& code)
	{
		PROFILE_FUNCTION();

		queue.enqueue(code);
	}
} // namespace SRC::auto_graph
