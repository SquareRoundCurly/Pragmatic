// Source header
#include "SubInterpreter.hpp"

// auto_graph
#include "./../auto_graph_cpp/Instrumentation.hpp"
#include "./../auto_graph_cpp/Out.hpp"

// External
#include "Python.h"
#include "readerwriterqueue.h"
using namespace moodycamel;

namespace
{
	using namespace Pragmatic::auto_graph;

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

	std::vector<PySubinterpreterObject*> subinterpreters;

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

	std::function<PyObject*()> PyRun(PyObject* callable, PyObject* args, PyObject* kwArgs)
	{
		return [=]() -> PyObject*
		{
			if (!PyCallable_Check(callable))
			{
				PyErr_SetString(PyExc_TypeError, "Expected a callable");
				return NULL;
			}

			auto* oldState = PyThreadState_Swap(pySubinterpreterTLS.threadState);
			
			PyObject* result = PyObject_Call(callable, args, kwArgs);

			PyThreadState_Swap(oldState);

			return result;
		};
	}
} // Anonymous namespace

namespace Pragmatic::auto_graph
{
	SubInterpreter::SubInterpreter()
	{
		// Prepare subinterpreters
		auto size = pool.Size();
		
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
	}
	
	SubInterpreter::~SubInterpreter()
	{
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
	}
	
	PyObject* SubInterpreter::Execute(PyObject* callable, PyObject* args, PyObject* kwArgs)
	{
		// Run code
		auto pyFuture = pool.Enqueue(PyRun(callable, args, kwArgs));

		return pyFuture.get();
	}
} // namespace Pragmatic::atuo_graph