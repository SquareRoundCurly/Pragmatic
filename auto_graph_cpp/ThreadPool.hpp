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


namespace Pragmatic::auto_graph
{
	class ThreadPool
	{
		public: // Constructors
		ThreadPool(size_t size = std::thread::hardware_concurrency());
		~ThreadPool();

		public: // ThreadPool
		template<class F, class... Args>
		auto Enqueue(F&& f, Args&&... args) -> std::future<decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))>;

		private: // State
		std::vector<std::thread> workers;
		std::queue<std::function<void()>> tasks;
	
		private: // Synchronization
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool stop;
	};

	template<class F, class... Args>
	auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))>
	{
		PROFILE_FUNCTION();

		using return_type = decltype(std::invoke(std::forward<F>(f), std::forward<Args>(args)...));

		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			// don't allow enqueueing after stopping the pool
			if(stop) throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks.emplace([task](){ (*task)(); });
		}
		condition.notify_one();
		return res;
	}
} // namespace Pragmatic::auto_graph