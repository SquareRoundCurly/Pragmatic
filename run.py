import auto_graph
import importlib

auto_graph.print('Hello')

importlib.reload(auto_graph)

auto_graph.print('Hello')

import sys
if 'auto_graph' in sys.modules:
    del sys.modules['auto_graph']
import auto_graph

auto_graph.print('Hello')

try:
	task = auto_graph.Task()
except Exception as e:
	print(f'Error: {e}')
# task = None
pass


def SomeFunc():
	auto_graph.print('Hello')

def SomeOtherFunc():
	value = 1
	for i in range(10):
		value = value + value
			
	auto_graph.print(value)

auto_graph.task(SomeFunc)
auto_graph.task(SomeOtherFunc)

auto_graph.exec()