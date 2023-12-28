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
#include <chrono>
using namespace std::chrono_literals;

namespace Pragmatic::auto_graph
{
	auto_graph_cpp::auto_graph_cpp()
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "Main module constructor" << std::endl;

		interpreters[0] = MainInterpreter::Get();
		interpreters[1] = new SubInterpreter();
		interpreters[2] = new ProcessInterpreter();
	}

	auto_graph_cpp::~auto_graph_cpp()
	{
		Out() << "[auto_graph] " << "Main module destructor" << std::endl;

	}

	PyObject* auto_graph_cpp::cleanup(PyObject* self, PyObject* args)
	{
		PROFILE_FUNCTION();

		Out() << "[auto_graph] " << "cleanup" << std::endl;

		static_cast<SubInterpreter*>(interpreters[1])->~SubInterpreter();
		delete interpreters[1];
		static_cast<ProcessInterpreter*>(interpreters[2])->~ProcessInterpreter();
		delete interpreters[2];
		
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

	thread_local struct PySubinterpreterTLS
	{
		PyThreadState* threadState = nullptr;
	} pySubinterpreterTLS;

	struct PySubinterpreterObject
	{
		PyThreadState* mainThreadState = nullptr;
		PyThreadState* subinterpreterThreadState = nullptr;

		BlockingReaderWriterQueue<std::function<void()>> queue;
		std::thread thread;
	};

	void CreatePySubinterpreter(PySubinterpreterObject* pySubinterpreter)
	{
		pySubinterpreter->mainThreadState = PyThreadState_Get();

		const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
		Py_NewInterpreterFromConfig(&pySubinterpreter->subinterpreterThreadState, &config);

		PyThreadState_Swap(pySubinterpreter->mainThreadState);
	}

	void DestructPySubinterpreter(PySubinterpreterObject* pySubinterpreter)
	{
		{
			PROFILE_SCOPE("Waiting for tasks to finish");
			
			pySubinterpreter->queue.enqueue(nullptr);

			if (pySubinterpreter->thread.joinable())
				pySubinterpreter->thread.join();
		}

		{
			PROFILE_SCOPE("Destorying subinterpreter");

			PyThreadState_Swap(pySubinterpreter->subinterpreterThreadState);
			Py_EndInterpreter(pySubinterpreter->subinterpreterThreadState);
			PyThreadState_Swap(pySubinterpreter->mainThreadState);
		}
	}

	void InitOnThread(PySubinterpreterObject* pySubinterpreter)
	{
		Out() << "Initializing subinterpreter on thread: " << std::this_thread::get_id() << std::endl;
		pySubinterpreterTLS.threadState = PyThreadState_New(pySubinterpreter->subinterpreterThreadState->interp);
		PyEval_RestoreThread(pySubinterpreterTLS.threadState);
	}

	void DestroyOnThread()
	{
		Out() << "destroying subinterpreter on thread: " << std::this_thread::get_id() << std::endl;
		PyThreadState_Clear(pySubinterpreterTLS.threadState);
		PyThreadState_DeleteCurrent();
	}

	std::function<void()> PyRun(const std::string& code)
	{
		return [=]()
		{
			auto* oldState = PyThreadState_Swap(pySubinterpreterTLS.threadState);
			
			PyObject* main_module = PyImport_AddModule("__main__");
			PyObject* global_dict = PyModule_GetDict(main_module);

			PyRun_String(code.c_str(), Py_file_input, global_dict, global_dict);
			
			PyThreadState_Swap(oldState);
		};
	}

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





		// Prepare subinterpreters
		auto size = pool.Size();
		std::vector<PySubinterpreterObject*> subinterpreters;
		for (size_t i = 0; i < size; i++)
		{
			auto* subinterpreter = new PySubinterpreterObject;
			subinterpreters.push_back(subinterpreter);
			CreatePySubinterpreter(subinterpreter);
		}

		// Init
		auto subinterpreterInits = pool.EnqueueForAll(subinterpreters, [](PySubinterpreterObject* subinterpreter) {
			InitOnThread(subinterpreter);
		});
		for (auto& result : subinterpreterInits)
			result.get();

		// Run code
		std::vector<std::shared_future<void>> pyResults;
		for (size_t i = 0; i < 100; i++)
		{
			auto pyFuture = pool.Enqueue(PyRun("import threading\nprint(f'" + std::to_string(i) + " : {threading.get_ident()}')"));
			pyResults.emplace_back(pyFuture.share());
		}
		for (auto& result : pyResults)
			result.get();

		// Destory on thread
		auto subinterpreterDestroy = pool.EnqueueForAll(subinterpreters, [](PySubinterpreterObject* subinterpreter) {
			DestroyOnThread();
		});
		for (auto& result : subinterpreterDestroy)
			result.get();

		// Cleanup subinterpreters
		for (auto& subinterpreter : subinterpreters)
		{
			DestructPySubinterpreter(subinterpreter);
			delete subinterpreter;
		}
		subinterpreters.clear();




		PySubinterpreterObject pySubinterpreterObject;
		CreatePySubinterpreter(&pySubinterpreterObject);

		pySubinterpreterObject.thread = std::thread([&]()
		{
			while (true)
			{
				std::function<void()> pythonTask;
				pySubinterpreterObject.queue.wait_dequeue(pythonTask);
				PyObject* result = nullptr;

                if (pythonTask == nullptr)
					break;
                else
					pythonTask();
			}
			
			Out() << "Exiting thread: " << std::this_thread::get_id() << std::endl;
		});

		pySubinterpreterObject.queue.enqueue([&pySubinterpreterObject]() { InitOnThread(&pySubinterpreterObject); });
		pySubinterpreterObject.queue.enqueue(PyRun("import threading\nprint(f'Hello: {threading.get_ident()}')"));
		pySubinterpreterObject.queue.enqueue(DestroyOnThread);

		DestructPySubinterpreter(&pySubinterpreterObject);

		Py_RETURN_NONE;
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