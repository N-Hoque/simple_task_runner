#include <iostream>
#include "task_runner.h"
#include "task_runner_serial.h"
#include <list>

int add(int x, int y)
{
	return x + y;
}

int sub(int x, int y)
{
	return x - y;
}

int mul(int x, int y)
{
	return x * y;
}

int mod(int x, int y)
{
	return x % y;
}

template<class TContainer>
void print_container(const TContainer &container)
{
	std::cout << "[";

	for (auto it = container.begin(); it != container.end(); ++it)
	{
		std::cout << *it;
		if (it == container.end() - 1)
		{
			break;
		}
		std::cout << ", ";
	}

	std::cout << "]\n";
}

int main(int argc, char *argv[])
{
	task_runner<int(int, int), 8> task_runner;

	// Fixed array of tasks.
	task_runner.set_tasks_fixed({ add, sub, mul, mod, add, sub, mul, mod });
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	std::array<std::function<int(int, int)>, 8>func_fixed_array{ sub,add, mod, mul, sub,add, mod, mul };

	// Fixed array of tasks using a variable.
	task_runner.set_tasks_fixed(func_fixed_array.begin(), func_fixed_array.end());
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	// Dynamic task setup. 
	// The two examples below show how you can use any STL container
	// by providing the just the container as a type argument.

	// Dynamic array (as vector)
	task_runner.set_tasks_dynamic<std::vector>({ mul, mod, add, sub, mul, mod, add, sub });
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	// Linked list
	task_runner.set_tasks_dynamic<std::list>({ mul, mod, add, sub, mul, mod, add, sub });
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	std::list<std::function<int(int, int)>>func_list{ mod, mul, sub, add, mod, mul, sub, add };

	task_runner.set_tasks_dynamic(func_list.begin(), func_list.end());
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	return 0;
}
