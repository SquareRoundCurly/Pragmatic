import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

auto_graph.initialize()

g = auto_graph.Graph()

a = g.add_node('a', 'print("Node: A")')
a.stuff = 'some data'

g.add_edge('a', 'b', 'print("A -> B")')
g.add_edge('a', 'c')
g.add_edge('b', 'd')

topo_gens = g.get_node_generations()

for generation_counter, gen in enumerate(topo_gens):
	print(f'Generation {generation_counter}: ', end='')
	
	for node in gen:
		print(node.get_name(), end=' ')
	
	print()

auto_graph.cleanup()

pass