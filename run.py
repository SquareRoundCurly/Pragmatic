import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

nodeA = auto_graph.Node('a', 'print("hello")')
nodeA.__exec()

nodeB = auto_graph.Node('b')

g = auto_graph.Graph()

g.add_edge(nodeA, nodeB)
g.add_edge(nodeB, 'c')

gens = g.get_node_generations()
for nodes in gens:
	for node in nodes:
		node.__exec()

auto_graph.cleanup()

pass