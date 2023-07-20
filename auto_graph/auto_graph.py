import atexit
import time
import auto_graph_cpp

# Register the cleanup function to be called at exit
# atexit.register(auto_graph_cpp.cleanup)

sleepy_code = """
import threading
import time
import random

time.sleep(random.uniform(0.1, 3))
print(f"Running on thread {threading.get_ident()}")
"""

for i in range(100):
	auto_graph_cpp.test(sleepy_code)

print('end')

auto_graph_cpp.cleanup()