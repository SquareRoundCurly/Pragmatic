import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

import auto_graph

node1 = auto_graph.Node('node 1')
node2 = auto_graph.Node('node 2')

print(node1.get_name())
print(node2.get_name())

g = auto_graph.Graph()

g.add_edge('a', 'b')
g.add_edge('a', 'c')
g.add_edge('b', 'd')

topo_gens = g.get_node_generations()

generation_counter = 0
for gen in topo_gens:
	for node in gen:
		print(f'[{generation_counter}] {node.get_name()}')
	generation_counter += 1

pass