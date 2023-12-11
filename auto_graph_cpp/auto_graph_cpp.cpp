// Source header
#include "auto_graph_cpp.hpp"

// External
#include <Python.h>

// auto_graph
#include "Out.hpp"
#include "PyRuntime/MainInterpreter.hpp"
#include "PyRuntime/SubInterpreter.hpp"
#include "PyRuntime/ProcessInterpreter.hpp"
#include "PyRuntime/PythonUtils.hpp"
#include "PyRuntime/PyRef.hpp"
#include "Instrumentation.hpp"

// TODO: remove this
#include "ThreadPool.hpp"
#include "readerwriterqueue.h"
using namespace moodycamel;
#include <string>
#include "Indexer.hpp"

namespace Pragmatic::auto_graph
{
	auto_graph_cpp::auto_graph_cpp()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "Main module constructor" << std::endl;

		interpreters.push_back(MainInterpreter::Get());
		interpreters.push_back(new SubInterpreter());
		interpreters.push_back(new ProcessInterpreter());
	}

	auto_graph_cpp::~auto_graph_cpp()
	{
		Out() << "[auto_graph] " << "Main module destructor" << std::endl;

		delete interpreters[1];
		delete interpreters[2];
		interpreters.clear();
	}

	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "cleanup" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::reinit(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "reinit" << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::print(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		PyObject* obj;
		if (!PyArg_ParseTuple(args, "O", &obj)) // Get an object
		{
			PyErr_SetString(PyExc_TypeError, "Failed to parse argument as object");
			return NULL;
		}

		PyRef str_obj = PyObject_Str(obj); // Get the string representation of the object
		if (!str_obj)
		{
			return NULL;  // PyObject_Str failed.
		}

		const char* str = PyUnicode_AsUTF8(str_obj);
		if (!str)
		{
			Py_DECREF(str_obj);
			return NULL;  // PyUnicode_AsUTF8 failed.
		}

		Out() << str << std::endl;

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::add_task(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		PyObject* callable;
		if (!PyArg_ParseTuple(args, "O", &callable))
		{
			return nullptr;
		}

		Py_INCREF(callable);
		tasks.push_back(callable);

		Py_RETURN_NONE;
	}

	PyObject* auto_graph_cpp::exec(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		for (PyRef task : tasks)
		{
			if (PyCallable_Check(task))
			{
				PyRef result = PyObject_CallObject(task, nullptr);
				if (!result)
				{
					PyErr_Print();
				}
			}
		}
		tasks.clear();

		Py_RETURN_NONE;
	}

	thread_local struct TLS
	{
		int i = -1;
	} tls;

	PyObject *auto_graph_cpp::test(PyObject *self, PyObject *args)
	{
		ThreadPool pool;

		auto res1 = pool.Enqueue([](){ return 5; });
		auto res2 = pool.Enqueue([](int b){ return 5 * b; }, 10);

		Out() << "res1 " << res1.get() << std::endl;
		Out() << "res2 " << res2.get() << std::endl;


		auto results = pool.EnqueueToAll([]() { Out() << "Running on thread: " << std::this_thread::get_id() << std::endl; });

		for (auto& result : results)
		{
			result.get();
		}

		std::vector<std::shared_future<void>> TLSresults;
		for (size_t i = 0; i < 20; i++)
		{
			auto TLSresult = pool.Enqueue([](TLS* tls)
			{
				if (tls->i == -1)
				{
					Out() << "Setting tls: " << std::this_thread::get_id() << std::endl;

					std::stringstream ss;
					ss << std::this_thread::get_id();
					tls->i = std::stoull(ss.str());
				}
				else
				{
					Out() << "tls ready:   " << std::this_thread::get_id() << std::endl;
				}
			}, &tls);
			
			TLSresults.push_back(TLSresult.share());
		}

		for (auto& result : TLSresults)
		{
			result.get();
		}
		
		std::vector<int> numbers = {1, 2, 3, 4, 5};
		auto resultsForEach = pool.EnqueueForEach(numbers, [](int num) {
			return num * 2; // Replace with your actual task logic
		});

		for (auto [i, result] : Indexer(resultsForEach))
		{
			Out() << "Result " << i << " : " << result.get() << std::endl;
		}


		// std::vector<PythonTask> pyTasks;
		// for (size_t i = 0; i < pool.Size(); i++)
		// {
		// 	pyTasks.emplace_back(PythonTask());
		// }

		// pool.Enqueue([&pyTask]()
		// {
		// 	pyTask.InitializeOnThread();
		// 	pyTask([]()
		// 	{
		// 		PyObject* main_module = PyImport_AddModule("__main__");
		// 		PyObject* global_dict = PyModule_GetDict(main_module);
		// 		auto result = PyRun_String("print('hello from PythonTask')", Py_eval_input, global_dict, global_dict);
		// 	});
		// 	pyTask.DestroyOnThread();
		// }).get();
		// pyTask.~PythonTask();

		Py_RETURN_NONE;

		/*
		auto mainThreadState = PyThreadState_Get();

		PyThreadState* subinterpreterThreadState = nullptr;
		const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
		Py_NewInterpreterFromConfig(&subinterpreterThreadState, &config);

		BlockingReaderWriterQueue<std::string> queue;

		auto thread = std::thread([&]()
		{
			auto newState = PyThreadState_New(subinterpreterThreadState->interp);
			PyEval_RestoreThread(newState);

			while (true)
			{
				std::string pythonTask;
				queue.wait_dequeue(pythonTask);
				PyObject* result = nullptr;

                if (pythonTask == "")
                    break;
                else
				{
					PROFILE_SCOPE("PyRun_String");

					auto& code = pythonTask;

					auto* oldState = PyThreadState_Swap(newState);

					PyObject* main_module = PyImport_AddModule("__main__");
					PyObject* global_dict = PyModule_GetDict(main_module);

					if (true)
						result = PyRun_String(code.c_str(), Py_eval_input, global_dict, global_dict);
					else
						result = PyRun_String(code.c_str(), Py_file_input, global_dict, global_dict);

					PyThreadState_Swap(oldState);
				}
			}
		
			PyThreadState_Clear(newState);
			PyThreadState_DeleteCurrent();
			
			Out() << "Exiting thread: " << std::this_thread::get_id() << std::endl;
		});

		PyThreadState_Swap(mainThreadState);

		// Add task
		queue.enqueue("print('Hello')");

		// Destruct
		{
			PROFILE_SCOPE("Waiting for tasks to finish");
			queue.enqueue("");

			if (thread.joinable())
				thread.join();
		}

		{
			PROFILE_SCOPE("Finalizing subinterpreter");
			PyThreadState_Swap(subinterpreterThreadState);
			Py_EndInterpreter(subinterpreterThreadState);
			PyThreadState_Swap(mainThreadState);
		}

		Py_RETURN_NONE;

		*/
	}

    int auto_graph_cpp::init(PyObject *module)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "init" << std::endl;

		return 0;
	}

	int auto_graph_cpp::traverse(PyObject* module, visitproc visit, void* arg)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "traverse" << std::endl;

		return 0;
	}

	int auto_graph_cpp::clear(PyObject* module)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "clear" << std::endl;

		return 0;
	}

	void auto_graph_cpp::free(void* module)
	{
		PROFILE_FUNCTION();
		
		Out() << "[auto_graph] " << "free" << std::endl;

		return;
	}
} // namespace Pragmatic::auto_graph