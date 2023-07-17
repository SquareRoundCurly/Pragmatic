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
		#pragma region Python scopes
		// Default handler concept, handlers restore states when out of scope
		template<typename Handler>
		concept HandlerConcept = requires(Handler handler, PyThreadState* state)
		{
			{ handler.Restore(state) } -> std::same_as<void>;
		};

		// Has Store function		
		template<typename Handler>
		concept HandlerHasStoreConcept = HandlerConcept<Handler> && requires(Handler handler, PyThreadState* state)
		{
			{ handler.Store() } -> std::same_as<PyThreadState*>;
		};
		template <typename H>
		using HasStore = std::enable_if_t<HandlerHasStoreConcept<H>, int>;

		// Has Swap function
		template<typename Handler>
		concept HandlerHasSwapConcept = HandlerConcept<Handler> && requires(Handler handler, PyThreadState* state)
		{
			{ handler.Swap(state) } -> std::same_as<PyThreadState*>;
		};
		template <typename H>
		using HasSwap = std::enable_if_t<HandlerHasSwapConcept<H>, int>;

		// Has Create function
		template<typename Handler>
		concept HandlerHasCreateConcept = HandlerConcept<Handler> && requires(Handler handler, PyInterpreterState* interpreter)
		{
			{ handler.Create(interpreter) } -> std::same_as<PyThreadState*>;
		};
		template <typename H>
		using HasCreate = std::enable_if_t<HandlerHasCreateConcept<H>, int>;

		template<HandlerConcept Handler>
		class ScopeGuard
		{
			public:
			~ScopeGuard() { handler.Restore(state); }
			 template<typename H = Handler, HasStore<H> = 0>
			 ScopeGuard() { state = handler.Store(); }
			 template<typename H = Handler, HasSwap<H> = 0>
			 ScopeGuard(PyThreadState* state) { this->state = handler.Swap(state); }
			 template<typename H = Handler, HasCreate<H> = 0>
			 ScopeGuard(PyInterpreterState* interpreter) { this->state = handler.Create(interpreter); }

			public:
			operator PyThreadState*() { return state; }
			static PyThreadState* Current() { return PyThreadState_Get(); }

			private:
			PyThreadState* state;
			Handler handler;
		};

		struct PyThread
		{
			PyThreadState* Store()                       { return PyEval_SaveThread(); }
			void           Restore(PyThreadState* state) { PyEval_RestoreThread(state); }
		};

		struct PyState
		{
			PyThreadState* Store()                       { return PyThreadState_Get(); }
			PyThreadState* Swap(PyThreadState* state)    { return PyThreadState_Swap(state); }
			void           Restore(PyThreadState* state) { PyThreadState_Swap(state); }
		};

		struct PyInterpreter
		{
			PyThreadState* Create(PyInterpreterState* interpreter)
			{
				auto state = PyThreadState_New(interpreter);
				PyEval_RestoreThread(state);
				return state;
			}

			void Restore(PyThreadState* state)
			{
				PyThreadState_Clear(state);
				PyThreadState_DeleteCurrent();
			}
		};
		#pragma endregion Python scopes

		void SetupGlobals(PyThreadState* substate, long ID)
		{
			PyObject* main_module = nullptr;
			PyObject* global_dict = nullptr;
			PyObject* id = nullptr;

			// PyThreadState_Swap(substate);

			main_module = PyImport_AddModule("__main__");
			if (main_module == NULL) goto error;

			global_dict = PyModule_GetDict(main_module);
			if (global_dict == NULL) goto error;

			id = PyLong_FromLong(ID);  // This is an int, not a long.
			if (id == NULL) goto error;

			// Set the integer as a global variable.
			if (PyDict_SetItemString(global_dict, "TNAME", id) < 0) goto error;

			// Decrease reference count for our integer object
			Py_DECREF(id);
			return;

			error: // Handle error & clean up any Python objects we created.
			Py_XDECREF(main_module);
			Py_XDECREF(global_dict);
			Py_XDECREF(id);
			return;
		}

		class SubInterpreter
		{
			public:
			SubInterpreter()
			{
				ScopeGuard<PyState> restore;

				const PyInterpreterConfig config = _PyInterpreterConfig_INIT;
				auto status = Py_NewInterpreterFromConfig(&threadState, &config);
			}

			~SubInterpreter()
			{
				if(threadState)
				{
					ScopeGuard<PyState> swap { threadState };
					Py_EndInterpreter(threadState);
				}
			}

			std::thread Run(const int ID, std::string& code)
			{
				return std::thread([this, ID, &code]()
				{
					auto script = std::string(code);
					script.replace(code.find("TNAME"), 5, std::to_string(ID));

					ScopeGuard<PyInterpreter> interpreter { threadState->interp };
					ScopeGuard<PyState> swap { interpreter };
					PyRun_SimpleString(script.c_str());
				});
			}

			std::thread RunFile(const int ID, const std::filesystem::path& file)
			{
				return std::thread([this, ID, &file]()
				{
					std::string fileStr = file.string();
					const char *fileCStr = fileStr.c_str();

					// Open file
					FILE* filePtr = std::fopen(fileCStr, "r");
					if (filePtr == nullptr)
					{
						std::cerr << "Could not open file " << file << '\n';
						return;
					}


					ScopeGuard<PyInterpreter> interpreter { threadState->interp };
					ScopeGuard<PyState> swap { interpreter };

					SetupGlobals(threadState, ID);
					PyRun_AnyFileEx(filePtr, fileCStr, 1);
				});
			}

			PyInterpreterState* interp() { return threadState->interp; }
			static PyInterpreterState* current() { return ScopeGuard<PyInterpreter>::Current()->interp; }

			private:
			PyThreadState* threadState;
		};
	} // Anonymous namespace

	PythonInterpreter::PythonInterpreter()
	{
		PyConfig config;
		PyConfig_InitPythonConfig(&config);
		config.isolated = 1;
		config.home = const_cast<wchar_t*>(CONCAT(L, PYTHON_ROOT_DIR));

		Py_InitializeFromConfig(&config);
		// PyConfig_Clear(&config);
	}

	PythonInterpreter::~PythonInterpreter()
	{
		Py_Finalize();
	}

	std::thread PythonInterpreter::Run(const int ID, std::string& code)
	{
		auto* _interpreter = PyThreadState_Get()->interp;
		return std::thread([this, ID, &code, _interpreter]()
		{
			auto script = std::string(code);
			script.replace(code.find("TNAME"), 5, std::to_string(ID));

			ScopeGuard<PyInterpreter> interpreter { _interpreter };
			ScopeGuard<PyState> swap { interpreter };
			PyRun_SimpleString(script.c_str());
		});
	}

std::string code = R"PY(
from __future__ import print_function
import sys
import threading

print(f"Running on thread {threading.get_ident()}")

print("TNAME: sys.xxx={}".format(getattr(sys, 'xxx', 'attribute not set')))
			)PY";

	void Test()
	{
		PythonInterpreter interpreter;

		SubInterpreter s1;
		SubInterpreter s2;

    	PyRun_SimpleString(R"PY(
# set sys.xxx, it will only be reflected in t4, which runs in the context of the main interpreter

from __future__ import print_function
import sys
import threading

print(f"Running on thread {threading.get_ident()}")

sys.xxx = ['abc']
print('main: setting sys.xxx={}'.format(sys.xxx))
		)PY");


		std::filesystem::path file = "auto_graph/subinterpreter_test.py";

		auto t1 = s1.RunFile(0, file);
		auto t2 = s2.Run(1, code);
		auto t3 = s2.Run(2, code);
		auto t4 = interpreter.Run(4, code);
		auto t5 = s1.Run(3, code);

		ScopeGuard<PyThread> guard;

		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();

		return;
	}
}