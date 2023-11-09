// Source header
#include "Instrumentation.hpp"

// Standard library
#include <fstream>
#include <atomic>

// Python
#include <Python.h>

// External
#include "concurrentqueue.h"
using namespace moodycamel;

// auto_graph
#include "Out.hpp"

namespace
{
	// Frame categories
	#define FRAME_CATEGORY \
		X(function)        \
		X(python_function)

	// Generate an enum with frame categories
	#define X(x) x,
	enum class FrameCategory : uint8_t { FRAME_CATEGORY number_of_categories };
	#undef X

	// Generate a string array with names of frame categories
	#define X(x) #x,
	const std::string FrameCategoryNames[] = { FRAME_CATEGORY };
	#undef X

	struct FrameData
	{
		std::string name;
		long long start;
		long long duration;
		uint32_t threadID;
		uint8_t category;
	};

	struct Session
	{
		std::ofstream file;
		ConcurrentQueue<FrameData> frames;
		std::atomic_bool isHeaderWritten = false;
		std::atomic_bool isFirstFrame = true;
		std::atomic<uint32_t> balance = 0;
		std::atomic_bool shouldExit = false;
	} session;

	void Flush()
	{
		FrameData frameData;
		while(session.frames.try_dequeue(frameData))
		{
			// Header
			if (!session.isHeaderWritten)
			{
				session.file << "{\"otherData\": {},\"traceEvents\":[";
				session.isHeaderWritten = true;
			}

			// Comma separator
			if (session.isFirstFrame)
				session.isFirstFrame = false;
			else
				session.file << ",";

			// Replace quatation marks in name
			std::string name = frameData.name;
			std::replace(name.begin(), name.end(), '"', '\'');

			// Frame data
			session.file << "{";
			session.file << "\"cat\":\"" << FrameCategoryNames[frameData.category] << "\",";
			session.file << "\"dur\":" << frameData.duration << ',';
			session.file << "\"name\":\"" << name << "\",";
			session.file << "\"ph\":\"X\",";
			session.file << "\"pid\":0,";
			session.file << "\"tid\":" << frameData.threadID << ",";
			session.file << "\"ts\":" << frameData.start;
			session.file << "}";
		}

		// Footer & close
		if (session.balance == 0)
		{
			session.file << "]}";
			session.file.flush();
			session.file.close();
		}
		else // Flush
			session.file.flush();
	}

	void AddFrame(const FrameData& frameData)
	{
		session.frames.enqueue(frameData);

		if (session.shouldExit)
			Flush();
	}
} // anonymous namespace

namespace Pragmatic::auto_graph
{
	void StartSession(const std::filesystem::path& filePath)
	{
		session.file.open(filePath);
		session.balance++;
	}

	void StopSession()
	{
		session.balance--;
		session.shouldExit = true;
		Flush();
	}

	ScopeTimer::ScopeTimer(const std::string& name, uint8_t category) : name(name), category(category)
	{
		start = std::chrono::high_resolution_clock::now();
		session.balance++;
	}

	ScopeTimer::~ScopeTimer()
	{
		auto end = std::chrono::high_resolution_clock::now();

		long long startMS = std::chrono::time_point_cast<std::chrono::microseconds>(start).time_since_epoch().count();
		long long endMS = std::chrono::time_point_cast<std::chrono::microseconds>(end).time_since_epoch().count();
		long long duration = endMS - startMS;

		uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());

		session.balance--;

		AddFrame({ name, startMS, duration, threadID, category });
	}

	int ScopeTimer::PyClassInit(PyClass* self, PyObject* args, PyObject* kwds)
	{
		char* c_name;

		if (!PyArg_ParseTuple(args, "s", &c_name)) return -1;

		std::string name(c_name);
		this->name = name;
		this->category = (uint8_t)FrameCategory::python_function;

		start = std::chrono::high_resolution_clock::now();
		session.balance++;

		return 0;
	}

	void ScopeTimer::PyClassDestruct(PyClass* self)
	{

	}
}