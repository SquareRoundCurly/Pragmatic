// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>
#include <stdexcept>
#include <chrono>
using namespace std::chrono_literals;

// External
#include <Python.h>

// auto_graph
#include "ScopeTimer.hpp"
#include "streams.hpp"

namespace
{
	using namespace SRC::auto_graph;

	std::vector<Subinterpreter*> interpreters;
} // anonymous namespace


namespace SRC::auto_graph
{
	void AddTask(const PythonTask& pythonTask)
	{
		auto task = pythonTask.task;
		if (std::holds_alternative<std::monostate>(task))
			throw std::runtime_error("Tried to execute empty task");

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

		idleInterpreter->Enque(pythonTask);
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
				PythonTask pythonTask;
				queue.wait_dequeue(pythonTask);
				PythonTaskVariant task = pythonTask.task;
				PyObject* result = nullptr;

				if (std::holds_alternative<std::filesystem::path>(task))
				{
					auto& filePath = std::get<std::filesystem::path>(task);

					auto* file = fopen(filePath.string().c_str(), "r");
					if(file != NULL)
					{
						PROFILE_SCOPE("PyRun_SimpleFile");

						auto* oldState = PyThreadState_Swap(newState);
						result = PyRun_File(file, filePath.string().c_str(), Py_file_input, nullptr, nullptr);
						PyThreadState_Swap(oldState);
						
						fclose(file);
					}
				}
				else if (std::holds_alternative<std::string>(task))
				{
					auto& code = std::get<std::string>(task);

					if (code == "__END__") break;

					{
						PROFILE_SCOPE("PyRun_SimpleString");

						auto* oldState = PyThreadState_Swap(newState);
						PyObject* main_module = PyImport_AddModule("__main__");
						PyObject* global_dict = PyModule_GetDict(main_module);
						result = PyRun_String(code.c_str(), Py_eval_input, global_dict, global_dict);
						PyThreadState_Swap(oldState);
					}
				}
				else if (std::holds_alternative<PyObject*>(task))
				{
					auto& callable = std::get<PyObject*>(task);

					{
						PROFILE_SCOPE("PyObject_CallObject");

						auto* oldState = PyThreadState_Swap(newState);
						result = PyObject_CallObject(callable, NULL);
						PyThreadState_Swap(oldState);
					}
				}
				else
				{
					// TODO: Error ...
				}

				// Check the result and convert it to bool
				if (result != nullptr)
				{
					if (PyBool_Check(result))
					{
						bool boolResult = PyObject_IsTrue(result);
						Py_DECREF(result); // Don't forget to DECREF when done

						// Now you have the boolean result, you can store or use it as you need.
						pythonTask.result.get()->set_value(boolResult);
					}
					else
					{
						Py_DECREF(result); // Don't forget to DECREF when done
						// Handle type mismatch error, maybe set an exception to the promise.
						pythonTask.result->set_exception(std::make_exception_ptr(std::runtime_error("Python execution returned non-bool type")));
					}
				}
				else
				{
					// Handle error, maybe set an exception to the promise.
					pythonTask.result.get()->set_exception(std::make_exception_ptr(std::runtime_error("Python execution failed")));
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
			queue.enqueue({std::string("__END__")});
			thread.join();
		}

		{
			PROFILE_SCOPE("Finalizing subinterpreter");
			PyThreadState_Swap(subinterpreterThreadState);
			Py_EndInterpreter(subinterpreterThreadState);
			PyThreadState_Swap(mainThreadState);
		}
	}

	void Subinterpreter::Enque(const PythonTask& task)
	{
		PROFILE_FUNCTION();

		queue.enqueue(task);
	}
} // namespace SRC::auto_graph
