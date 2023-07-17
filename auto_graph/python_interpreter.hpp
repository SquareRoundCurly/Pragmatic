#pragma once

// Standard library
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <filesystem>
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
		void RunOnMain(std::string& code);
		void RunOnMain(const std::filesystem::path& file);
		std::thread Run(const int ID, std::string& code);
		std::thread Run(const int ID, const std::filesystem::path& file);
	};
} // namespace SRC::AG