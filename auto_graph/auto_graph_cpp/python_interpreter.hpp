#pragma once

// Standard library
#include <thread>
#include <string>

// External
#include "readerwriterqueue.h"
using namespace moodycamel;

// auto_graph
#include "PythonTask.hpp"

// Forward decalartions
struct _ts;
typedef struct _ts PyThreadState;

namespace SRC::auto_graph
{
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