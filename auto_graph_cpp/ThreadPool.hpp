#pragma once

// Standard library
#include <thread>
#include <future>
#include <vector>
#include <condition_variable>
#include <functional>

// TODO: remove this
#include <queue>
#include <mutex>

// auto_graph
#include "Instrumentation.hpp"

// External
#include "blockingconcurrentqueue.h"


namespace Pragmatic::auto_graph
{
	class Worker
	{
		public:
		Worker();
		~Worker();

		public:
		void Run();
		void EnqueueTask(std::function<void()> task);
		inline size_t GetApproximateSize() { return queue.size_approx(); }

		private:
		std::thread thread;
		moodycamel::BlockingConcurrentQueue<std::function<void()>> queue;
	};

	class ThreadPool
	{
		public: // Constructors
		ThreadPool(size_t size = std::thread::hardware_concurrency());
		~ThreadPool();

		public: // ThreadPool
		template<class F, class... Args>
		auto Enqueue(F&& f, Args&&... args) -> std::future<decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))>;

		private: // State
		moodycamel::BlockingConcurrentQueue<std::function<void()>> queue;
		std::vector<std::unique_ptr<Worker>> workers;
	};

	template <class F, class... Args>
	inline auto ThreadPool::Enqueue(F &&f, Args &&...args) -> std::future<decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))>
	{
		PROFILE_FUNCTION();

		using return_type = decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();

		static size_t lastWorkerID = 0;
		size_t lowestQueueSize = LLONG_MAX;
		Worker* selectedWorker = nullptr;

		for (const auto& worker : workers)
		{
			size_t queueSize = worker->GetApproximateSize();
			if (queueSize < lowestQueueSize)
			{
				lowestQueueSize = queueSize;
				selectedWorker = worker.get();
			}
		}

		selectedWorker->EnqueueTask([task]() { (*task)(); });
		return res;
	}
} // namespace Pragmatic::auto_graph