#pragma once

// Standard library
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

#ifdef _MSC_VER
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define CONCAT_IMPL(x, y) x##y // Helper macro to concatenate two tokens
#define CONCAT(x, y) CONCAT_IMPL(x, y) // Macro to concatenate two tokens

namespace SRC::Debug
{
	struct ProfileResult
	{
		std::string name;
		long long start, end;
		uint32_t threadID;
	};

	class Instrumentor
	{
		private:
		std::ofstream m_OutputStream;
		int m_ProfileCount;

		public:
		Instrumentor() : m_ProfileCount(0) { }

		void BeginSession(const std::string& filepath = "results.json")
		{
			m_OutputStream.open(filepath);
			WriteHeader();
		}

		void EndSession()
		{
			WriteFooter();
			m_OutputStream.close();
			m_ProfileCount = 0;
		}

		void WriteProfile(const ProfileResult& result)
		{
			if (m_ProfileCount++ > 0) m_OutputStream << ",";

			std::string name = result.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			m_OutputStream << "{";
			m_OutputStream << "\"cat\":\"function\",";
			m_OutputStream << "\"dur\":" << (result.end - result.start) << ',';
			m_OutputStream << "\"name\":\"" << name << "\",";
			m_OutputStream << "\"ph\":\"X\",";
			m_OutputStream << "\"pid\":0,";
			m_OutputStream << "\"tid\":" << result.threadID << ",";
			m_OutputStream << "\"ts\":" << result.start;
			m_OutputStream << "}";

			m_OutputStream.flush();
		}

		void WriteHeader()
		{
			m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
			m_OutputStream.flush();
		}

		void WriteFooter()
		{
			m_OutputStream << "]}";
			m_OutputStream.flush();
		}

		static Instrumentor& Get()
		{
			static Instrumentor instance;
			return instance;
		}
	};

	class InstrumentationTimer
	{
		public:
		InstrumentationTimer(const char* name) : m_Name(name), m_Stopped(false)
		{
			m_StartTimepoint = std::chrono::high_resolution_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_Stopped) Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::high_resolution_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
			Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

			m_Stopped = true;
		}

		private:
		const char* m_Name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
		bool m_Stopped;
	};

} // namespace SRC::Debug

#ifdef _DEBUG
#define PROFILE 1
#else
#define PROFILE 0
#endif


#if PROFILE
	#define PROFILE_BEGIN_SESSION(name, path)	::SRC::Debug::Instrumentor::Get().BeginSession(path)
	#define PROFILE_END_SESSION()				::SRC::Debug::Instrumentor::Get().EndSession()
	#define PROFILE_SCOPE(name)					::SRC::Debug::InstrumentationTimer CONCAT(timer, __LINE__)(name)
	#define PROFILE_FUNCTION()					PROFILE_SCOPE(__PRETTY_FUNCTION__)
#else
	#define PROFILE_BEGIN_SESSION(name, path)
	#define PROFILE_END_SESSION()
	#define PROFILE_FUNCTION()
	#define PROFILE_SCOPE(name)
#endif