import os
import auto_graph
import unittest

def print_thread():
	import threading
	auto_graph.print(f"print_thread function running on thread {threading.get_ident()}")

class BasicTest(unittest.TestCase):
	def test_0_build(self):
		sleepy_code = """
import threading
import time
import random

time.sleep(random.uniform(0.01, 0.1))
print(f"Running on thread {threading.get_ident()}")

__return = True
"""

		try:
			for i in range(20):
				auto_graph.task(sleepy_code)

			auto_graph.task('tests/1_basic/some_other_script.py')
			auto_graph.task(print_thread)
		except Exception as error:
			self.fail(error)