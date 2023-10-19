#pragma once

// Standard library
#include <atomic>

// External
#include <pytypedefs.h>


namespace SRC::auto_graph
{
	struct InterpreterState
	{
		PyObject *an_object;

		static InterpreterState* Get();
	};

	class AutoGraphState
	{
		private:
		AutoGraphState() = default;
		public:
		static AutoGraphState* Get();

		public:
		PyObject* GetModule() const;
		void SetModule(PyObject* module);
		inline void IncrementInitCounter() { init_counter++; }
		inline void DecrementInitCounter() { init_counter--; }
		inline uint8_t GetInitCounter() { return init_counter; }

		private:
		PyObject* auto_graph_module;
		std::atomic<int16_t> init_counter = 0;
	};
} // namespace SRC::auto_graph