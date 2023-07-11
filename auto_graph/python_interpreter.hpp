#pragma once

// Standard library
#include <vector>
#include <string>
#include <memory>
#include <atomic>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <deque>

// Forward declarations
struct _ts;
using PyThreadState = _ts;
namespace SRC::AG
{
	class PythonSubinterpreter;
}

namespace SRC::AG
{
	void Test();

	class PythonInterpreter
	{
		public:
		 PythonInterpreter();
		~PythonInterpreter();

		public:
		std::thread Run(const int ID, std::string& code);
	};
	// class PythonInterpreter
	// {
	// 	friend class PythonSubinterpreter;

	// 	public:
	// 	PythonInterpreter();
	// 	~PythonInterpreter();
	// 	void CreateSubinterpreter();
	// 	void Run(int index, const std::string& script);

	// 	private:
	// 	PyThreadState* mainstate;
	// 	std::vector<std::unique_ptr<PythonSubinterpreter>> subinterpreters;
	// 	std::atomic_bool isRunning = true;
	// };

	// class PythonSubinterpreter
	// {
	// 	public:
	// 	PythonSubinterpreter(const PythonInterpreter& pythonInterpreter, const size_t index);
	// 	~PythonSubinterpreter();
	// 	void Run(const std::string& script);

	// 	private:
	// 	const PythonInterpreter& pythonInterpreter;
	// 	const size_t index = -1;
	// 	PyThreadState* substate;
	// };
} // namespace SRC::AG