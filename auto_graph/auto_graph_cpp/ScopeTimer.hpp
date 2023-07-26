#pragma once

// Standard library
#include <string>
#include <chrono>
#include <filesystem>

// auto_graph
#include "Macros.hpp"

// Forward declarations
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
	void StartSession(const std::filesystem::path& filePath);
	void StopSession();

	class ScopeTimer
	{
		public:
		 ScopeTimer(const std::string& name, uint8_t category = 0);
		~ScopeTimer();

		private:
		const std::string name;
		const uint8_t category;
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	};

	int Register(PyObject* module);
} // namespace SRC::auto_graph

#define PROFILE

#ifdef PROFILE
	#define PROFILE_BEGIN_SESSION(path)			::SRC::auto_graph::StartSession(path)
	#define PROFILE_END_SESSION()				::SRC::auto_graph::StopSession()
	#define PROFILE_SCOPE(name)					::SRC::auto_graph::ScopeTimer CONCAT(timer, __LINE__)(name)
	#define PROFILE_FUNCTION()					PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
	#define PROFILE_BEGIN_SESSION(path)
	#define PROFILE_END_SESSION()
	#define PROFILE_FUNCTION()
	#define PROFILE_SCOPE(name)
#endif