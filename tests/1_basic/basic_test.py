import os
import auto_graph
import unittest

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
				# TODO: This still doesn't work on github CI
				pass # auto_graph.task(sleepy_code)

			# auto_graph.task('tests/1_basic/some_other_script.py')
			auto_graph.task(print_thread)
		except Exception as error:
			self.fail(error)