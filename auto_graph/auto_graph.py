import atexit
import time
import auto_graph_cpp

# Register the cleanup function to be called at exit
# atexit.register(auto_graph_cpp.cleanup)

auto_graph_cpp.init()

auto_graph_cpp.test('print("hello")')
auto_graph_cpp.test(
    """
for	i in range(10):
    print(i)
	"""
)

auto_graph_cpp.cleanup()

print('end')