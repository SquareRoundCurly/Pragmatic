import os
import auto_graph
import unittest
from pathlib import Path

def print_thread() -> bool:
	import threading
	auto_graph.print(f"print_thread function running on thread {threading.get_ident()}")
	return True

class BasicTest(unittest.TestCase):
	def test_0_build(self):
		sleepy_code = """
import threading
import time

time.sleep(0.1)
print(f"Running on thread {threading.get_ident()}")

__return = True
"""

		try:
			for i in range(20):
				auto_graph.task(sleepy_code)

			# TODO: This still doesn't work on github CI
			path = Path(__file__).parent.joinpath('some_other_script.py')
			auto_graph.task(str(path))
		except Exception as error:
			self.fail(error)