import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

g = auto_graph.Graph()

a = g.add_node('a', 'True and False')
a.stuff = 'some data'
a.__exec()
print(f'Result A: {a.get_result()}')

edge = g.add_edge('a', 'b', 'True')

g.add_edge('a', 'c')
g.add_edge('b', 'd')

topo_gens = g.get_node_generations()

for generation_counter, gen in enumerate(topo_gens):
	print(f'Generation {generation_counter}: ', end='')
	
	for node in gen:
		print(node.get_name(), end=' ')
	
	print()

edge.__exec()
print(f'Edge A->B: {edge.get_result()}')
auto_graph.cleanup()

pass