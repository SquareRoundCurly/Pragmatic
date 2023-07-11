// Source header
#include "python_interpreter.hpp"

// Standard library
#include <iostream>

// External
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// auto_graph
#include "instrument.hpp"

namespace SRC::AG
{
	namespace
	{
		// template<typename T, typename GetFunc, typename RestoreFunc>
		// class ScopeGuard
		// {
		// 	public:
		// 	 ScopeGuard() { stored = GetFunc(); }
		// 	~ScopeGuard() { RestoreFunc(stored); }

		// 	private:
		// 	T* stored;
		// };
		// using PythonThreadScopeGuard = ScopeGuard<PyThreadState, PyEval_SaveThread, PyEval_RestoreThread>;

		struct PythonInitialize
		{
			PythonInitialize()
			{
				PyConfig config;
				PyConfig_InitPythonConfig(&config);
				config.isolated = 1;
				config.home = const_cast<wchar_t*>(CONCAT(L, PYTHON_ROOT_DIR));

				Py_InitializeFromConfig(&config);
				// PyConfig_Clear(&config);
			}
			~PythonInitialize() { Py_Finalize(); }
		};

		class PythonThreadScopeGuard
		{			
			public:
			 PythonThreadScopeGuard() { state = PyEval_SaveThread(); }
			~PythonThreadScopeGuard() { PyEval_RestoreThread(state); }

			private:
			PyThreadState* state;
		};

		class PythonThreadStateScopeGuard
		{
			public:
			 PythonThreadStateScopeGuard() { threadState = PyThreadState_Get(); }
			~PythonThreadStateScopeGuard() { PyThreadState_Swap(threadState); }

			private:
			PyThreadState* threadState;
		};

		class PythonSwapStateScopeGuard
		{
			public:
			 PythonSwapStateScopeGuard(PyThreadState* ts) { threadState = PyThreadState_Swap(ts); }
			~PythonSwapStateScopeGuard()                  { PyThreadState_Swap(threadState); }

			private:
			PyThreadState* threadState;
		};

		class PythonThreadState
		{
			public:
			PythonThreadState(PyInterpreterState* interp)
			{
				threadState = PyThreadState_New(interp);
				PyEval_RestoreThread(threadState);
			}

			~PythonThreadState()
			{
				PyThreadState_Clear(threadState);
				PyThreadState_DeleteCurrent();
			}

			operator PyThreadState*() { return threadState; }
			static PyThreadState* Current() { return PyThreadState_Get(); }

			private:
			PyThreadState* threadState;
		};

		class sub_interpreter
		{
			public:
			struct thread_scope
			{
				thread_scope(PyInterpreterState* interp) : threadState(interp) { }
				PythonThreadState threadState;
				PythonSwapStateScopeGuard swap { threadState };
			};

			public:
			sub_interpreter()
			{
				PythonThreadStateScopeGuard restore;
				threadState = Py_NewInterpreter();
			}

			~sub_interpreter()
			{
				if(threadState)
				{
					PythonSwapStateScopeGuard sts(threadState);
					Py_EndInterpreter(threadState);
				}
			}

			PyInterpreterState* interp() { return threadState->interp; }
			static PyInterpreterState* current() { return PythonThreadState::Current()->interp; }

			private:
			PyThreadState* threadState;
		};

		void f(PyInterpreterState* interp, const char* tname)
		{
			std::string code = R"PY(
from __future__ import print_function
import sys

print("TNAME: sys.xxx={}".format(getattr(sys, 'xxx', 'attribute not set')))
			)PY";

			code.replace(code.find("TNAME"), 5, tname);

			sub_interpreter::thread_scope scope(interp);
			PyRun_SimpleString(code.c_str());
		}

		void ReleaseGIL()
		{
			PyThreadState_Swap(NULL);
		}

		void SetupGlobals(PyThreadState* substate, long ID)
		{
			PyObject* main_module = nullptr;
			PyObject* global_dict = nullptr;
			PyObject* id = nullptr;

			PyThreadState_Swap(substate);

			main_module = PyImport_AddModule("__main__");
			if (main_module == NULL) goto error;

			global_dict = PyModule_GetDict(main_module);
			if (global_dict == NULL) goto error;

			id = PyLong_FromLong(ID);  // This is an int, not a long.
			if (id == NULL) goto error;

			// Set the integer as a global variable.
			if (PyDict_SetItemString(global_dict, "ID", id) < 0) goto error;

			// Decrease reference count for our integer object
			Py_DECREF(id);
			return;

			error: // Handle error & clean up any Python objects we created.
			Py_XDECREF(main_module);
			Py_XDECREF(global_dict);
			Py_XDECREF(id);
			return;
		}
	} // Anonymous namespace

	void Test()
	{
		PythonInitialize init;

		sub_interpreter s1;
		sub_interpreter s2;

    	PyRun_SimpleString(R"PY(
# set sys.xxx, it will only be reflected in t4, which runs in the context of the main interpreter

from __future__ import print_function
import sys

sys.xxx = ['abc']
print('main: setting sys.xxx={}'.format(sys.xxx))
		)PY");

		std::thread t1{f, s1.interp(), "t1(s1)"};
		std::thread t2{f, s2.interp(), "t2(s2)"};
		std::thread t3{f, s1.interp(), "t3(s1)"};
		std::thread t4{f, sub_interpreter::current(), "t4(main)"};

		PythonThreadScopeGuard t;

		t1.join();
		t2.join();
		t3.join();
		t4.join();
		
		return;
	}

	// PythonInterpreter::PythonInterpreter()
	// {
	// 	PyConfig config;
	// 	PyConfig_InitPythonConfig(&config);
	// 	config.home = const_cast<wchar_t*>(CONCAT(L, PYTHON_ROOT_DIR));

	// 	auto status = Py_InitializeFromConfig(&config);
	// 	if (PyStatus_Exception(status))
	// 	{
	// 		std::cout << "Couldn't initialize Python" << std::endl;
	// 	}

	// 	mainstate = PyThreadState_Get();
	// }

	// PythonInterpreter::~PythonInterpreter()
	// {
	// 	isRunning = false;

	// 	// End subinterpreters
	// 	subinterpreters.clear();
		
	// 	// End main interpreter
	// 	PyThreadState_Swap(mainstate);
	// 	Py_Finalize();
	// }

	// void PythonInterpreter::CreateSubinterpreter()
	// {
	// 	subinterpreters.emplace_back(std::make_unique<PythonSubinterpreter>(*this, subinterpreters.size()));
	// }

	// void PythonInterpreter::Run(int index, const std::string& script)
	// {
	// 	subinterpreters[index]->Run(script);
	// }

	// PythonSubinterpreter::PythonSubinterpreter(const PythonInterpreter& pythonInterpreter, const size_t index)
	// : pythonInterpreter(pythonInterpreter), index(index)
	// {
	// 	const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
		
	// 	ReleaseGIL();

	// 	PyStatus status = Py_NewInterpreterFromConfig(&substate, &config);
	// 	if (PyStatus_Exception(status))
	// 	{
	// 		/* Since no new thread state was created, there is no exception to
	// 		propagate; raise a fresh one after swapping in the old thread
	// 		state. */
	// 		PyThreadState_Swap(pythonInterpreter.mainstate);
	// 		_PyErr_SetFromPyStatus(status);
	// 		PyObject *exc = PyErr_GetRaisedException();
	// 		PyErr_SetString(PyExc_RuntimeError, "sub-interpreter creation failed");
	// 		_PyErr_ChainExceptions1(exc);
	// 		return;
	// 	}

	// 	assert(substate != NULL);

	// 	SetupGlobals(substate, (long)index);
	// }

	// PythonSubinterpreter::~PythonSubinterpreter()
	// {
	// 	PyThreadState_Swap(substate);
	// 	Py_EndInterpreter(substate);
	// }

	// void PythonSubinterpreter::Run(const std::string& script)
	// {
	// 	PyCompilerFlags cflags = {0};
	// 	PyThreadState_Swap(substate);
	// 	auto r = PyRun_SimpleStringFlags(script.c_str(), &cflags);
	// }
}