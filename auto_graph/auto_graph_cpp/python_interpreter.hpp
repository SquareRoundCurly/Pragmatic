#pragma once

// Standard library
#include <thread>
#include <string>
#include <filesystem>
#include <variant>
#include <future>

// External
#include "readerwriterqueue.h"
using namespace moodycamel;

// Forward decalartions
struct _ts;
typedef struct _ts PyThreadState;
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
	using PythonTaskVariant = std::variant<std::monostate, std::filesystem::path, std::string, PyObject*>; // Python file, string or callable
	using PythonResult = std::shared_ptr<std::promise<bool>>;
	struct PythonTask
	{
		PythonTaskVariant task;
		PythonResult result;

		PythonTask(PythonTaskVariant task = std::monostate()) : task(std::move(task)), result(std::make_shared<std::promise<bool>>()) { }
	};

	void AddTask(const PythonTask& task);
	void Initialize();
	void Cleanup();

	class Subinterpreter
	{
		public:
		Subinterpreter();
		~Subinterpreter();

		public:
		void Enque(const PythonTask& task);
		inline size_t GetSize() { return queue.size_approx(); }

		private:
		PyThreadState* mainThreadState;
		PyThreadState* subinterpreterThreadState;
		std::thread thread;
		BlockingReaderWriterQueue<PythonTask> queue;
	};
} // namespace SRC::auto_graph