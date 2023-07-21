import unittest
import auto_graph

class BasicTest(unittest.TestCase):
	def test_0_build(self):
		try:
			sleepy_code = """
import threading
import time
import random

time.sleep(random.uniform(0.1, 3))
print(f"Running on thread {threading.get_ident()}")
"""
			for i in range(20):
				auto_graph.task(sleepy_code)

			auto_graph.cleanup()
		except Exception as error:
			self.fail(error)