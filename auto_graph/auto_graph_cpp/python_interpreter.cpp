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
#include "Graph.hpp"

namespace
{
	using namespace SRC::auto_graph;

	std::vector<Subinterpreter*> interpreters;

	enum class CodeType
	{
		None,
		Expression,
		TopLevel
	};

	CodeType ClassifyCode(const std::string& code)
	{
		PyObject* astModule = PyImport_ImportModule("ast");
		if (astModule == nullptr)
		{
			Out() << "Failed to import ast module" << std::endl;
			return CodeType::None;
		}

		PyObject* parseFunc = PyObject_GetAttrString(astModule, "parse");
		if (parseFunc == nullptr)
		{
			Py_DECREF(astModule);
			Out() << "Failed to get ast.parse" << std::endl;
			return CodeType::None;
		}

		PyObject* codeObj = PyUnicode_FromString(code.c_str());
		if (codeObj == nullptr)
		{
			Py_DECREF(parseFunc);
			Py_DECREF(astModule);
			Out() << "Failed to create Python string" << std::endl;
			return CodeType::None;
		}

		PyObject* args = PyTuple_Pack(1, codeObj);
		if (args == nullptr)
		{
			Py_DECREF(codeObj);
			Py_DECREF(parseFunc);
			Py_DECREF(astModule);
			Out() << "Failed to create arguments tuple" << std::endl;
			return CodeType::None;
		}

		// Try parsing as an expression
		PyObject* result = PyObject_Call(parseFunc, args, Py_BuildValue("{s:s}", "mode", "eval"));
		if (result != nullptr)
		{
			Py_DECREF(result);
			Py_DECREF(args);
			Py_DECREF(codeObj);
			Py_DECREF(parseFunc);
			Py_DECREF(astModule);
			return CodeType::Expression;
		}
		PyErr_Clear(); // Clear the exception

		// Try parsing as top-level code
		result = PyObject_Call(parseFunc, args, Py_BuildValue("{s:s}", "mode", "exec"));
		if (result != nullptr)
		{
			Py_DECREF(result);
			Py_DECREF(args);
			Py_DECREF(codeObj);
			Py_DECREF(parseFunc);
			Py_DECREF(astModule);
			return CodeType::TopLevel;
		}

		PyErr_Clear(); // Clear the exception

		// Clean up
		Py_DECREF(args);
		Py_DECREF(codeObj);
		Py_DECREF(parseFunc);
		Py_DECREF(astModule);

		Out() << "Invalid Python code" << std::endl;
		return CodeType::None;
	}

	PyObject* GetArtificialReturnValue(PyObject* result, PyObject* globalDict)
	{
		
		if (result != nullptr)
		{
			PyObject* returnValue = PyDict_GetItemString(globalDict, "__return");
			if (returnValue != nullptr && PyBool_Check(returnValue))
			{
				// No need to DECREF returnValue, PyDict_GetItemString returns a borrowed reference
				return returnValue;
			}
			else
			{
				// Handle the case where __return is not set or is not a bool
				throw std::runtime_error("Python execution did not set __return to a bool");
			}
		}
		else throw std::runtime_error("Python execution returned null!");
	}

	size_t GetNumberOfArgs(PyObject* callable)
	{
		size_t paramCount = 0;
		
		PyObject* codeObj = PyObject_GetAttrString(callable, "__code__");
		if (codeObj != NULL)
		{
			PyObject* argCountObj = PyObject_GetAttrString(codeObj, "co_argcount");
			if (argCountObj != NULL)
			{
				paramCount = PyLong_AsLong(argCountObj);
				Py_DECREF(argCountObj);
			}
			Py_DECREF(codeObj);
		}

		return paramCount;
	}
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

						PyObject* main_module = PyImport_AddModule("__main__");
						PyObject* global_dict = PyModule_GetDict(main_module);
						
						result = PyRun_File(file, filePath.string().c_str(), Py_file_input, global_dict, nullptr);
						result = GetArtificialReturnValue(result, global_dict);

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

						auto codeType = ClassifyCode(code);
						switch (codeType)
						{
						case CodeType::Expression:
							result = PyRun_String(code.c_str(), Py_eval_input, global_dict, global_dict);
							break;
						
						case CodeType::TopLevel:
							result = PyRun_String(code.c_str(), Py_file_input, global_dict, global_dict);
							result = GetArtificialReturnValue(result, global_dict);
							break;

						default:
							break;
						}

						PyThreadState_Swap(oldState);
					}
				}
				else if (std::holds_alternative<PyObject*>(task))
				{
					auto& callable = std::get<PyObject*>(task);

					{
						PROFILE_SCOPE("PyObject_CallObject");

						auto* oldState = PyThreadState_Swap(newState);

						PyObject* sourceNode = nullptr;
						PyObject* targetNode = nullptr;

						if (pythonTask.source != "")
							sourceNode = pythonTask.graph->pyNodes[pythonTask.source];
						if (pythonTask.target != "")
							targetNode = pythonTask.graph->pyNodes[pythonTask.target];

						if (PyCallable_Check(callable))
						{
							auto paramCount = GetNumberOfArgs(callable);

							// Depending on paramCount, call callable with appropriate arguments
							if (paramCount == 2)
							{
								PyObject* args = PyTuple_Pack(2, sourceNode, targetNode);
								result = PyObject_CallObject(callable, args);
								Py_DECREF(args);
							}
							else if (paramCount == 1)
							{
								PyObject* args = PyTuple_Pack(1, sourceNode);
								result = PyObject_CallObject(callable, args);
								Py_DECREF(args);
							}
							else
							{
								result = PyObject_CallObject(callable, NULL);
							}
						}
						else
						{
							throw std::exception("Tried to execute non callable python object!");
						}

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
