#pragma once

// Standard library
#include <string>
#include <chrono>
#include <filesystem>

// auto_graph
#include "PyRuntime/PyClass.hpp"
#include "Macros.hpp"

// External
#include "pytypedefs.h"

namespace Pragmatic::auto_graph
{
	void StartSession(const std::filesystem::path& filePath);
	void StopSession();

	class ScopeTimer : public PyClass
	{
		public:
		ScopeTimer() = default;
		ScopeTimer(const std::string& name, uint8_t category = 0);
		~ScopeTimer();

		public:
		virtual int PyClassInit(PyClass* self, PyObject* args, PyObject* kwds) override;
		virtual void PyClassDestruct(PyClass* self) override;

		private:
	    std::string name;
		uint8_t category;
		std::chrono::time_point<std::chrono::high_resolution_clock> start;
	};
} // namespace Prgmatic::auto_graph

#define PROFILE

#ifdef PROFILE
	#define PROFILE_BEGIN_SESSION(path)			::Pragmatic::auto_graph::StartSession(path)
	#define PROFILE_END_SESSION()				::Pragmatic::auto_graph::StopSession()
	#define PROFILE_SCOPE(name)					::Pragmatic::auto_graph::ScopeTimer CONCAT(timer, __LINE__)(name)
	#define PROFILE_FUNCTION()					PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
	#define PROFILE_BEGIN_SESSION(path)
	#define PROFILE_END_SESSION()
	#define PROFILE_FUNCTION()
	#define PROFILE_SCOPE(name)
#endif