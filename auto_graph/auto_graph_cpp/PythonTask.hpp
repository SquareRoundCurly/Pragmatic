#pragma once

// Standard library
#include <memory>
#include <variant>
#include <future>
#include <filesystem>

// Fowrard declarations
struct _object;
typedef struct _object PyObject;

namespace SRC::auto_graph
{
	using PythonTaskVariant = std::variant<std::monostate, std::filesystem::path, std::string, PyObject*>; // Python file, string or callable
	using PythonResult = std::shared_ptr<std::promise<bool>>;
	struct PythonTask
	{
		PythonTaskVariant task;
		PythonResult result;

		PythonTask(PythonTaskVariant task = std::monostate()) : task(std::move(task)), result(std::make_shared<std::promise<bool>>()) { }
	};
}