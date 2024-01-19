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

	uint64_t GetThreadID()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return std::stoull(ss.str());
	}

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

	uint64_t InitOnThread(PySubinterpreterObject* pySubinterpreter)
	{
		pySubinterpreterTLS.threadState = PyThreadState_New(pySubinterpreter->subinterpreterThreadState->interp);
		PyEval_RestoreThread(pySubinterpreterTLS.threadState);

		return GetThreadID();
	}

	uint64_t DestroyOnThread()
	{
		PyThreadState_Clear(pySubinterpreterTLS.threadState);
		PyThreadState_DeleteCurrent();

		return GetThreadID();
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

			Py_INCREF(result);
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
		std::mutex mtx;
		std::vector<uint64_t> ids;
		auto subinterpreterInits = pool.EnqueueForAll(subinterpreters, [&mtx, &ids](PySubinterpreterObject* subinterpreter)
		{
			auto id = InitOnThread(subinterpreter);

			{
				std::lock_guard lock(mtx);
				ids.push_back(id);
			}
		});

		// Wait for inits to return from all threads
		for (auto& result : subinterpreterInits)
			result.get();

		// Print init message
		std::stringstream ss;
		if (!ids.empty())
		{
			ss << "Initialized " << size << " subinterpreters:";
			for (const uint64_t& id : ids)
				ss << " " << id;
		}
		else ss << "No subinterpreters initialized";
   		Out() << "[auto_graph] " << ss.str() << std::endl;
	}
	
	SubInterpreter::~SubInterpreter()
	{
		// Destory on thread
		std::mutex mtx;
		std::vector<uint64_t> ids;
		auto subinterpreterDestroy = pool.EnqueueForAll(subinterpreters, [&mtx, &ids](PySubinterpreterObject* subinterpreter)
		{
			auto id = DestroyOnThread();

			{
				std::lock_guard lock(mtx);
				ids.push_back(id);
			}
		});
		
		// Wait for destroys to return from all threads
		for (auto& result : subinterpreterDestroy)
			result.get();

		// Print destroy message
		std::stringstream ss;
		if (!ids.empty())
		{
			ss << "Destroyed " << ids.size() << " subinterpreters:";
			for (const uint64_t& id : ids)
				ss << " " << id;
		}
		else ss << "No subinterpreters destroyed on threads";
   		Out() << "[auto_graph] " << ss.str() << std::endl;

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