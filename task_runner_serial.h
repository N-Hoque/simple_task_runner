/*
 * Author: Naimul Hoque (Naimul_Hoque@outlook.com)
 * Date Created: 7th January 2020
 */

#ifndef TASK_RUNNER_SERIAL_H
#define TASK_RUNNER_SERIAL_H

#include <cstdlib>
#include <functional>
#include <array>
#include <vector>

 /*
  * A class for running multiple tasks sequentially.
  */
template<class TFunction, std::size_t NUM_OF_TASKS>
class task_runner_serial
{
private:
	using TReturn = typename std::function<TFunction>::result_type;

	std::array<std::function<TFunction>, NUM_OF_TASKS> m_tasks;
	std::array<TReturn, NUM_OF_TASKS> m_results{};
public:
	task_runner_serial() = default;

	/*
	 * Sets the tasks here. This is for compile-time task setup
	 */
	void set_tasks_fixed(const std::array<std::function<TFunction>, NUM_OF_TASKS> &functions)
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_tasks[i] = std::function<TFunction>(functions[i]);
		}
	}

	/*
	 * Sets the tasks here. This is for runtime task setup
	 */
	void set_tasks_dynamic(const std::vector<std::function<TFunction>> &functions)
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_tasks[i] = std::function<TFunction>(functions[i]);
		}
	}

	/*
	 * This runs all the tasks. Tasks must have been set up before
	 * running this method.
	 */
	template<typename... TArgs>
	void run_tasks(TArgs... args)
	{
		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			m_results[i] = m_tasks[i](args...);
		}
	}

	/*
	 * Returns the results after running the tasks.
	 */
	std::array<TReturn, NUM_OF_TASKS> get_results()
	{
		return m_results;
	}

	/*
	 * Reduces the results into a single value after running the tasks.
	 * Do not run get_results and this method together, otherwise an error will occur.
	 */
	TReturn reduce_results()
	{
		TReturn reduced_result{};

		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			reduced_result = reduced_result + m_results[i];
		}

		return reduced_result;
	}

	/*
	 * Reduces the results into a single value after running the tasks.
	 * This uses a custom combine function for reducing the results.
	 * Do not run get_results and this method together, otherwise an error will occur.
	 */
	TReturn reduce_results(TReturn(*combine)(TReturn, TReturn))
	{
		TReturn reduced_result{};

		for (std::size_t i{}; i < NUM_OF_TASKS; i++)
		{
			reduced_result = combine(reduced_result, m_results[i]);
		}

		return reduced_result;
	}
};

#endif