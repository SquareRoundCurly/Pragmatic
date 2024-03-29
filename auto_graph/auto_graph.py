# Imports
import ctypes

try:
    from .__private import auto_graph_cpp
except ImportError as e:
    print(e)
    ctypes.windll.kernel32.SetLastError(0)
    ctypes.windll.kernel32.GetLastError()
    print(ctypes.WinError())
import importlib

# At exit callback
import atexit
atexit.register(auto_graph_cpp.cleanup)

# auto_graph methods

def cleanup() -> None:
	auto_graph_cpp.cleanup()

def print(msg: str) -> None:
	auto_graph_cpp.print(msg)

def task(code: str) -> None:
	auto_graph_cpp.task(code)

def exec() -> None:
	auto_graph_cpp.exec()

def test() -> None:
	auto_graph_cpp.test()

# Handle module reload
__original_reload = importlib.reload
def __auto_graph_reload(module):
	is_auto_graph = module.__name__ == 'auto_graph'

	# Check if the module being reloaded is 'auto_graph'
	if is_auto_graph:
		# Pre-reload cleanup code
		auto_graph_cpp.print('[auto_graph] pre-reload')
		auto_graph_cpp.cleanup()
		atexit.unregister(auto_graph_cpp.cleanup)

		# Explicitly reload the auto_graph_cpp module
		__original_reload(auto_graph_cpp)
		
	# Call the original reload function
	__original_reload(module)
	
	# Post-reload code
	if is_auto_graph:
		auto_graph_cpp.print('[auto_graph] post-reload')
		# Re-register with atexit
		atexit.register(auto_graph_cpp.cleanup)
		auto_graph_cpp.reinit()
		
	return module
# Override the original reload
importlib.reload = __auto_graph_reload