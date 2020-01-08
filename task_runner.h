/*
 * Author: Naimul Hoque (Naimul_Hoque@outlook.com)
 * Date Created: 7th January 2020
 */

#ifndef TASK_RUNNER_H
#define TASK_RUNNER_H

#include <array>
#include <future>
#include <thread>
#include <functional>
#include <type_traits>

 /*
  * A class for running multiple tasks on multiple threads
  * that have the same function signature.
  */
template <class TFunction, size_t NUM_OF_TASKS>
class task_runner
{
private:
	using Function = std::function<TFunction>;

	using Result = typename Function::result_type;

	using Task = std::packaged_task<TFunction>;
	using Future = std::future<Result>;
	using Thread = std::thread;

	template <class T>
	using FixedArray = std::array<T, NUM_OF_TASKS>;

	using ReduceFunction = Result(*)(Result, Result);

	FixedArray<Task>   m_tasks;
	FixedArray<Future> m_futures{};
	FixedArray<Thread> m_threads;
	FixedArray<Result> m_results{};

public:
	task_runner() = default;

	/*
	 * Sets the tasks here. This is for compile-time task setup
	 */
	void set_tasks_fixed(FixedArray<Function> &&functions)
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_tasks[i] = Task(functions[i]);
			m_futures[i] = m_tasks[i].get_future();
		}
	}

	/*
	 * Sets the tasks here. This is for compile-time task setup
	 */
	template <typename TIterator>
	void set_tasks_fixed(const TIterator &functions_begin, const TIterator &functions_end)
	{
		size_t i{};
		for (TIterator it = functions_begin; it != functions_end; ++it)
		{
			m_tasks[i] = Task(*it);
			m_futures[i] = m_tasks[i].get_future();
			++i;
		}
	}

	/*
	 * Sets the tasks here. This is for runtime task setup.
	 * Takes as a type argument anything that implements an iterator.
	 */
	template<template<class...> class TContainer>
	void set_tasks_dynamic(TContainer<Function> &&functions)
	{
		size_t i{};
		for (auto it = functions.begin(); it != functions.end(); ++it)
		{
			m_tasks[i] = Task(*it);
			m_futures[i] = m_tasks[i].get_future();
			++i;
		}
	}

	/*
	 * Sets the tasks here. This is for runtime task setup
	 */
	template <typename TIterator>
	void set_tasks_dynamic(const TIterator &functions_begin, const TIterator &functions_end)
	{
		size_t i{};
		for (TIterator it = functions_begin; it != functions_end; ++it)
		{
			m_tasks[i] = Task(*it);
			m_futures[i] = m_tasks[i].get_future();
			++i;
		}
	}

	/*
	 * This runs all the tasks. Tasks must have been set up before
	 * running this method.
	 */
	template <typename... TArgs>
	void run_tasks(TArgs ...args)
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_threads[i] = Thread(std::move(m_tasks[i]), args...);
			m_threads[i].join();
		}
	}

	/*
	 * Returns an array of futures after running the tasks.
	 */
	FixedArray<Future> get_futures()
	{
		return m_futures;
	}

	/*
	 * Returns the results of the futures after running the tasks.
	 */
	FixedArray<Result> get_results()
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_results[i] = m_futures[i].get();
		}

		return m_results;
	}

	/*
	 * Reduces the results into a single value after running the tasks.
	 * Do not run get_results and this method together, otherwise an error will occur.
	 */
	Result reduce_results()
	{
		Result reduced_result{};

		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			reduced_result = reduced_result + m_futures[i].get();
		}

		return reduced_result;
	}

	/*
	 * Reduces the results into a single value after running the tasks.
	 * This uses a custom combine function for reducing the results.
	 * Do not run get_results and this method together, otherwise an error will occur.
	 */
	Result reduce_results(const ReduceFunction combine)
	{
		Result reduced_result{};

		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			reduced_result = combine(reduced_result, m_futures[i].get());
		}

		return reduced_result;
	}
};

#endif
