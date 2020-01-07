#ifndef TASK_RUNNER_H
#define TASK_RUNNER_H

#include <array>
#include <vector>
#include <future>
#include <thread>
#include <functional>
#include <type_traits>

/*
 * A class for running multiple tasks on multiple threads
 * that have the same function signature.
 */
template<class TFunction, size_t NUM_OF_TASKS>
class task_runner
{
private:
	using TReturn = typename std::function<TFunction>::result_type;

	std::array<std::packaged_task<TFunction>, NUM_OF_TASKS> m_tasks;
	std::array<std::future<TReturn>, NUM_OF_TASKS> m_futures;
	std::array<std::thread, NUM_OF_TASKS> m_threads;
	std::array<TReturn, NUM_OF_TASKS> m_results{};

public:
	task_runner() = default;

	/*
	 * Sets the tasks here. This is for compile-time task setup
	 */
	void set_tasks_fixed(const std::array<std::function<TFunction>, NUM_OF_TASKS> &functions)
	{
		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			m_tasks[i] = std::packaged_task<TFunction>(functions[i]);
			m_futures[i] = m_tasks[i].get_future();
		}
	}

	void set_tasks_dynamic(const std::vector<std::function<TFunction>> &functions)
	{
		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			m_tasks[i] = std::packaged_task<TFunction>(functions[i]);
			m_futures[i] = m_tasks[i].get_future();
		}
	}

	template<typename... TArgs>
	void run_tasks(TArgs... args)
	{
		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			m_threads[i] = std::thread(std::move(m_tasks[i]), args...);
			m_threads[i].join();
		}
	}

	std::array<std::future<TReturn>, NUM_OF_TASKS> get_futures()
	{
		return m_futures;
	}

	std::array<TReturn, NUM_OF_TASKS> get_results()
	{
		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			m_results[i] = m_futures[i].get();
		}

		return m_results;
	}

	TReturn reduce_results()
	{
		TReturn reduced_result{};

		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			reduced_result = reduced_result + m_futures[i].get();
		}

		return reduced_result;
	}

	TReturn reduce_results(TReturn(*combine)(TReturn, TReturn))
	{
		TReturn reduced_result{};

		for (auto i = 0ULL; i < NUM_OF_TASKS; i++)
		{
			reduced_result = combine(reduced_result, m_futures[i].get());
		}

		return reduced_result;
	}
};

#endif