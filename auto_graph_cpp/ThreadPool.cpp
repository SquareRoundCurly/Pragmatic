// Source header
#include "ThreadPool.hpp"

namespace Pragmatic::auto_graph
{
	ThreadPool::ThreadPool(size_t size) : stop(false)
	{
		PROFILE_FUNCTION();

		for(size_t i = 0; i < size; ++i)
		{
			PROFILE_SCOPE("Pragmatic::auto_graph::ThreadPool::construct thread");

			workers.emplace_back
			(
				[this]
				{
					for(;;)
					{
						std::function<void()> task;

						{
							std::unique_lock<std::mutex> lock(this->queue_mutex);
							this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });

							if(this->stop && this->tasks.empty())
								return;
							
							task = std::move(this->tasks.front());
							this->tasks.pop();
						}

						PROFILE_SCOPE("Pragmatic::auto_graph::ThreadPool::task_execution");

						task();
					}
				}
			);
		}
	}

	ThreadPool::~ThreadPool()
	{
		PROFILE_FUNCTION();

		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			stop = true;
		}

		condition.notify_all();
		for(std::thread &worker: workers)
			worker.join();
	}
} // namespace Pragmatic::auto_graph