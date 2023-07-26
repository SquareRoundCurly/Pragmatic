import os
import auto_graph
import unittest

class BasicTest(unittest.TestCase):
	def test_0_build(self):
		sleepy_code = """
import threading
import time
import random
# import auto_graph_cpp

time.sleep(random.uniform(0.1, 3))
print(f"Running on thread {threading.get_ident()}")
"""

		try:
			auto_graph.initialize()

			for i in range(20):
				auto_graph.task(sleepy_code)

			auto_graph.task('tests/1_basic/some_other_script.py')

			auto_graph.cleanup()
		except Exception as error:
			self.fail(error)