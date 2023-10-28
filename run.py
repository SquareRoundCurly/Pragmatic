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

def test(i: int, j: int):
	return i + j

t = auto_graph.Task(test, 1000, 2000)
auto_graph.print(t.exec(3000))
t = None

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



def kwargs_func(a, b=2, **kwargs):
	return a + b + kwargs.get('c', 0)
t = auto_graph.Task(kwargs_func, 1)
result = t.exec(b=3, c=4)
pass