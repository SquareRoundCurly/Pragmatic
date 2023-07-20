
// Standard library
#include <thread>
#include <string>

// External
#include "readerwriterqueue.h"
using namespace moodycamel;

// Forward decalartions
struct _ts;
typedef struct _ts PyThreadState;

namespace SRC::auto_graph
{
	void Test(const std::string& code);
	void Initialize();
	void Cleanup();

	class Subinterpreter
	{
		public:
		Subinterpreter();
		~Subinterpreter();

		public:
		void Enque(const std::string& code);
		inline size_t GetSize() { return queue.size_approx(); }

		private:
		PyThreadState* mainThreadState;
		PyThreadState* subinterpreterThreadState;
		std::thread thread;
		BlockingReaderWriterQueue<std::string> queue;
	};
} // namespace SRC::auto_graph