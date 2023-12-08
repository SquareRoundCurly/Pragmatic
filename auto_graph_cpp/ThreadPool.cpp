// Source header
#include "ThreadPool.hpp"

namespace Pragmatic::auto_graph
{
	Worker::Worker()
	{
		thread = std::thread([this]
		{
			this->Run();
		});
	}

	Worker::~Worker()
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	void Worker::Run()
	{
		while (true)
		{
			std::function<void()> task;
			queue.wait_dequeue(task);  // Blocking dequeue
			if (!task) break;  // nullptr task signifies shutdown
			task();
		}
	}

	void Worker::EnqueueTask(std::function<void()> task)
	{
		queue.enqueue(task);
	}

	ThreadPool::ThreadPool(size_t size) : workers()
	{
		PROFILE_FUNCTION();

		for(size_t i = 0; i < size; ++i)
		{
			workers.emplace_back(std::make_unique<Worker>());
		}
	}

	ThreadPool::~ThreadPool()
	{
		PROFILE_FUNCTION();

		for (const auto& worker : workers)
		{
			worker->EnqueueTask(nullptr);  // Send shutdown signal
		}
	}
} // namespace Pragmatic::auto_graph