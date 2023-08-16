import auto_graph

import unittest

class BasicTest(unittest.TestCase):
	def test_0_graph(self):
		g = auto_graph.Graph()

		g.add_edge('a', 'b')
		g.add_edge('a', 'c')
		g.add_edge('b', 'd')
		g.add_edge('c', 'e')
		g.add_edge('d', 'f')
		g.add_edge('e', 'f')
		g.add_edge('f', 'g')
		g.add_edge('f', 'h')
		g.add_edge('c', 'i')
		g.add_edge('i', 'j')

		g.print_topological_generations()