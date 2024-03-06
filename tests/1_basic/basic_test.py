import unittest
import importlib
from multiprocessing import Process
from typing import List

def reimport_test_func():
	import sys
	from pathlib import Path
	sys.path.append(str(Path(__file__).parent.parent.parent.absolute()))

	try:
		import auto_graph
	except:
		print("couldn't import auto_graph")

	auto_graph.print('1')

	importlib.reload(auto_graph)

	auto_graph.print('2')

	import sys
	if 'auto_graph' in sys.modules:
		del sys.modules['auto_graph']
	import auto_graph

	auto_graph.print('3')

class BasicTest(unittest.TestCase):
	def test_0_imports(self):
		processes: List[Process] = []

		# Start 5 subprocesses
		for _ in range(5):
			p = Process(target=reimport_test_func)
			p.start()
			processes.append(p)

		# Wait for all subprocesses to finish
		for p in processes:
			if p.is_alive():
				p.join()

		# Check exit codes
		for p in processes:
			self.assertEqual(p.exitcode, 0, "A subprocess terminated with an error")