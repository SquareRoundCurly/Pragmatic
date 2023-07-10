// Standard library
#include <thread>
#include <chrono>
using namespace std::chrono_literals;

// auto_graph
#include "instrument.hpp"
#include "python_interpreter.hpp"

int main()
{
	auto pi = SRC::AG::PythonInterpreter();
	pi.CreateSubinterpreter();
	pi.Run(0, "print(f'Hello {ID}')");
	pi.CreateSubinterpreter();
	pi.Run(1, "print(f'Hello {ID}')");
	pi.Run(0, "print(f'Hello World ! {ID}')");

	return 0;
}