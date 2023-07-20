
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
	void Test(const std::string& str);

	class Subinterpreter
	{
		public:
		Subinterpreter();
		~Subinterpreter();

		public:
		void Run(const std::string& code);

		private:
		PyThreadState* mainThreadState;
		PyThreadState* subinterpreterThreadState;
		std::thread thread;
		BlockingReaderWriterQueue<std::string> queue;
	};
} // namespace SRC::auto_graph