#include <iostream>
#include "task_runner.h"
#include "task_runner_serial.h"

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
	task_runner<int(int, int), 4> task_runner;

	task_runner.set_tasks_fixed({ add, sub, mul, mod });
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	task_runner.set_tasks_fixed({ mod, mul, sub, add });
	task_runner.run_tasks(5, 10);
	print_container(task_runner.get_results());

	return 0;
}
