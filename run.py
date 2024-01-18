import auto_graph

def reimport_test_func():
	import sys
	import importlib
	from pathlib import Path
	sys.path.append(str(Path(__file__).parent.parent.parent.absolute()))

	try:
		import auto_graph
	except:
		print("couldn't import auto_graph")

	importlib.reload(auto_graph)

	import sys
	if 'auto_graph' in sys.modules:
		del sys.modules['auto_graph']
	import auto_graph

timer = auto_graph.ScopeTimer('run.py')


g = auto_graph.Graph()
g.add_node('a')
g.add_node('b')
g.add_node('c')
g.add_node('d')
g.add_node('e')
g.add_edge('a', 'b')
g.add_edge('a', 'c')
g.add_edge('b', 'd')
g.add_edge('b', 'e')
gens = g.topological_generations()

for	gen in gens:
	line_str = ''
	for node in gen:
		line_str += node + ' '
	print(line_str)

auto_graph.print(g)

def test_func(a, b):
	return a + b
t = auto_graph.Task(test_func, 1)
result = t.exec(2, 3, 4)
auto_graph.print(result)

def nested():
	def test_func_nested(a, b):
		return a + b
	t = auto_graph.Task(test_func_nested, 1)
	result = t.exec(2, 3, 4)
	auto_graph.print(result)
nested()

del timer


reimport_test_func()

pass