import auto_graph

import unittest

class NodesTest(unittest.TestCase):
	def test_0_node_references(self):
		g = auto_graph.Graph()

		data = 'Some other data'

		a = g.add_node('a')
		a.dynamic_property = data

		g.add_node('b')
		g.add_node('c')
		g.add_node('d')

		node = g.get_node('a')
		self.assertEqual(node.dynamic_property, data)

	def test_1_topological_generations(self):
		g = auto_graph.Graph()

		g.add_edge('a', 'b')
		g.add_edge('a', 'c')
		g.add_edge('b', 'd')

		topo_gens = g.get_node_generations()

		for generation_counter, gen in enumerate(topo_gens):
			print(f'Generation {generation_counter}: ', end='')
			
			for node in gen:
				print(node.get_name(), end=' ')
			
			print()

		# Assertions
		self.assertEqual([node.get_name() for node in topo_gens[0]], ['a'])
		self.assertCountEqual([node.get_name() for node in topo_gens[1]], ['b', 'c'])
		self.assertEqual([node.get_name() for node in topo_gens[2]], ['d'])