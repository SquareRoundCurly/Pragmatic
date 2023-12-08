#pragma once

// Standard library
#include <thread>
#include <future>
#include <vector>
#include <condition_variable>
#include <functional>

// auto_graph
#include "Instrumentation.hpp"

// External
#include "blockingconcurrentqueue.h"
using namespace moodycamel;


namespace Pragmatic::auto_graph
{
	template<class F, class... Args>
	using TaskReturnType = std::invoke_result_t<F, Args...>;
	template<class F, class... Args>
	using TaskFuture = std::future<TaskReturnType<F, Args...>>;
	template<class F, class... Args>
	using PackagedTask = std::packaged_task<TaskReturnType<F, Args...>()>;

	template <class F, class... Args>
	inline auto CreateTaskAndFuture(F&& f, Args&&... args)
	{
		auto task = std::make_shared<PackagedTask<F, Args...>>
		(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		auto res = task->get_future();

		return std::make_pair(std::move(task), std::move(res));
	}

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
		BlockingConcurrentQueue<std::function<void()>> queue;
	};

	class ThreadPool
	{
		public: // Constructors
		ThreadPool(size_t size = std::thread::hardware_concurrency());
		~ThreadPool();

		public: // Get
		inline const size_t Size() const { return workers.size(); } 

		public: // ThreadPool
		template<class F, class... Args>
		auto Enqueue(F&& f, Args&&... args) -> TaskFuture<F, Args...>;
		template<class F, class... Args>
		auto EnqueueToAll(F&& f, Args&&... args) -> std::vector<TaskFuture<F, Args...>>;
		template <class Container, class F, class... Args>
		auto EnqueueForEach(Container&& container, F&& f, Args&&... args) -> std::vector<TaskFuture<F, typename std::decay_t<decltype(*container.begin())>, Args...>>;

		private: // State
		BlockingConcurrentQueue<std::function<void()>> queue;
		std::vector<std::unique_ptr<Worker>> workers;
	};

	template <class F, class... Args>
	inline auto ThreadPool::Enqueue(F &&f, Args &&...args) -> TaskFuture<F, Args...>
	{
		PROFILE_FUNCTION();

		auto [task, res] = CreateTaskAndFuture(std::forward<F>(f), std::forward<Args>(args)...);

		auto selectedWorker = std::min_element(workers.begin(), workers.end(), [](const auto& a, const auto& b)
		{
			return a->GetApproximateSize() < b->GetApproximateSize();
		});

		(*selectedWorker)->EnqueueTask([task]() { (*task)(); });

		return std::move(res);
	}

	template <class F, class... Args>
	inline auto ThreadPool::EnqueueToAll(F&& f, Args&&... args) -> std::vector<TaskFuture<F, Args...>>
	{
		PROFILE_FUNCTION();

		std::vector<TaskFuture<F, Args...>> results;

		for (const auto& worker : workers)
		{
			auto [task, res] = CreateTaskAndFuture(std::forward<F>(f), std::forward<Args>(args)...);

			worker->EnqueueTask([task]() { (*task)(); });

			results.push_back(std::move(res));
		}

		return results;
	}

	template <class Container, class F, class... Args>
	inline auto ThreadPool::EnqueueForEach(Container&& container, F&& f, Args&&... args) -> std::vector<TaskFuture<F, typename std::decay_t<decltype(*container.begin())>, Args...>>
	{
		PROFILE_FUNCTION();

		using ContainerValueType = typename std::decay_t<decltype(*container.begin())>;
		std::vector<TaskFuture<F, ContainerValueType, Args...>> results;
		results.reserve(container.size());

		for (const auto& item : container)
		{
			auto [task, res] = CreateTaskAndFuture(
				std::forward<F>(f),
				item,
				std::forward<Args>(args)...
			);

			auto selectedWorker = std::min_element(workers.begin(), workers.end(), [](const auto& a, const auto& b)
			{
				return a->GetApproximateSize() < b->GetApproximateSize();
			});

			(*selectedWorker)->EnqueueTask([task]() { (*task)(); });

			results.push_back(std::move(res));
		}

		return results;
	}
} // namespace Pragmatic::auto_graph