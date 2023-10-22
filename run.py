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




def SomeFunc():
    auto_graph.print('Hello')

def SomeOtherFunc():
	value = 1
	for i in range(10):
		value = value + value
			
	print(value)

auto_graph.task(SomeFunc)
auto_graph.task(SomeOtherFunc)

auto_graph.exec()