import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

g = auto_graph.Graph()

a = g.add_node('a', 'print("hello")')
a.stuff = 'a'
print(a.stuff)

b = g.get_node('a')
print(b.stuff)

g.add_node('b')

gens = g.get_node_generations()
for nodes in gens:
	for node in nodes:
		node.__exec()
		if node.get_name() == 'a':
			print(node.stuff)

a.__exec()

auto_graph.cleanup()

pass