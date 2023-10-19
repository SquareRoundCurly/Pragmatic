// Standard library
#include <iostream>
#include <mutex>

namespace Pragmatic::auto_graph
{
	class Out
	{
		private:
		static std::mutex mutex;

		public:
		template<typename T>
		Out& operator<<(const T& data)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << data;
			std::cout.flush();
			return *this;
		}

		Out& operator<<(std::ostream& (*pf)(std::ostream&))
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << pf;
			return *this;
		}
	};
	inline std::mutex Out::mutex;
} // namespace SRC::auto_graph