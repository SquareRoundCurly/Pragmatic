// Source header
#include "ScopeTimer.hpp"

// Standard library
#include <fstream>
#include <atomic>

// Python
#include <Python.h>

// External
#include "concurrentqueue.h"
using namespace moodycamel;

// auto_graph
#include "../streams.hpp"

namespace
{
	// Frame categories
	#define FRAME_CATEGORY \
		X(function)        \
		X(python_function)

	// Generate an enum with fruits
	#define X(fruit) fruit,
	enum class FrameCategory : uint8_t { FRAME_CATEGORY number_of_categories };
	#undef X

	// Generate a string array with names of fruits
	#define X(fruit) #fruit,
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
		bool isHeaderWritten = false;
		bool isFirstFrame = true;
		std::atomic<uint32_t> balance = 0;
		bool shouldExit = false;
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

namespace SRC::auto_graph
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

	#pragma region ScopeTimer

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

	#pragma endregion ScopeTimer

	#pragma region Python type

	typedef struct
	{
		PyObject_HEAD
		SRC::auto_graph::ScopeTimer* scopeTimer;
	} PyScopeTimer;

	static int PyScopeTimer_init(PyScopeTimer *self, PyObject *args, PyObject *kwds)
	{
		char* c_name;

		if (!PyArg_ParseTuple(args, "s", &c_name)) return -1;

		std::string name(c_name);
		self->scopeTimer = new ScopeTimer(name, (uint8_t)FrameCategory::python_function);

		return 0;
	}

	static void PyScopeTimer_dealloc(PyScopeTimer* self)
	{
		delete self->scopeTimer;
		Py_TYPE(self)->tp_free((PyObject*)self);
	}

	static PyTypeObject PyScopeTimerType =
	{
		PyVarObject_HEAD_INIT(NULL, 0)
		"auto_graph_cpp.ScopeTimer",              /* tp_name */
		sizeof(PyScopeTimer),                     /* tp_basicsize */
		0,                                        /* tp_itemsize */
		(destructor)PyScopeTimer_dealloc,         /* tp_dealloc */
		0,                                        /* tp_print */
		0,                                        /* tp_getattr */
		0,                                        /* tp_setattr */
		0,                                        /* tp_reserved */
		0,                                        /* tp_repr */
		0,                                        /* tp_as_number */
		0,                                        /* tp_as_sequence */
		0,                                        /* tp_as_mapping */
		0,                                        /* tp_hash  */
		0,                                        /* tp_call */
		0,                                        /* tp_str */
		0,                                        /* tp_getattro */
		0,                                        /* tp_setattro */
		0,                                        /* tp_as_buffer */
		Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
		"ScopeTimer objects",                     /* tp_doc */
		0,                                        /* tp_traverse */
		0,                                        /* tp_clear */
		0,                                        /* tp_richcompare */
		0,                                        /* tp_weaklistoffset */
		0,                                        /* tp_iter */
		0,                                        /* tp_iternext */
		0,                                        /* tp_methods */
		0,                                        /* tp_members */
		0,                                        /* tp_getset */
		0,                                        /* tp_base */
		0,                                        /* tp_dict */
		0,                                        /* tp_descr_get */
		0,                                        /* tp_descr_set */
		0,                                        /* tp_dictoffset */
		(initproc)PyScopeTimer_init,              /* tp_init */
		0,                                        /* tp_alloc */
		PyType_GenericNew,                        /* tp_new */
	};

	int Register(PyObject* module)
	{
		if (PyType_Ready(&PyScopeTimerType) < 0) return -1;

		Py_INCREF(&PyScopeTimerType);
		if (PyModule_AddObject(module, "ScopeTimer", (PyObject *)&PyScopeTimerType) < 0)
		{
			Py_DECREF(&PyScopeTimerType);
			return -1;
    	}
	}
	
	#pragma endregion Python type
} // namespace SRC::auto_graph