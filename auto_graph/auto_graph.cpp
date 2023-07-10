// Standard library
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// auto_graph
#include "instrument.hpp"
#include "python_interpreter.hpp"

int main()
{
	std::cout << "Hello world !" << std::endl;

	auto pi = SRC::AG::PythonInterpreter();
	pi.CreateSubinterpreter();
	pi.Run(0);
	pi.CreateSubinterpreter();
	pi.Run(1);
	pi.Run(0);

	return 0;
}