
// Standard library
#include <thread>
#include <string>
#include <filesystem>
#include <variant>

// External
#include "readerwriterqueue.h"
using namespace moodycamel;

// Forward decalartions
struct _ts;
typedef struct _ts PyThreadState;

namespace SRC::auto_graph
{
	using FileOrCode = std::variant<std::filesystem::path, std::string>;

	void AddTask(const FileOrCode& fileOrCode);
	void Initialize();
	void Cleanup();

	class Subinterpreter
	{
		public:
		Subinterpreter();
		~Subinterpreter();

		public:
		void Enque(const FileOrCode& fileOrCode);
		inline size_t GetSize() { return queue.size_approx(); }

		private:
		PyThreadState* mainThreadState;
		PyThreadState* subinterpreterThreadState;
		std::thread thread;
		BlockingReaderWriterQueue<FileOrCode> queue;
	};
} // namespace SRC::auto_graph