import auto_graph

timer = auto_graph.ScopeTimer('run.py')

g = auto_graph.Graph()
g.add_node('asdf')

auto_graph.print(g)

def test_func(a, b):
	return a + b
t = auto_graph.Task(test_func, 1)
result = t.exec(2, 3, 4)
auto_graph.print(result)

del timer
pass