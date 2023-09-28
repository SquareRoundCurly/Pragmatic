import auto_graph
import unittest

def compile():
	print('Compiling')
	return True

def link():
	print('Linking')

class NodesTest(unittest.TestCase):
	def test_0_node_references(self):
		g = auto_graph.Graph()

		g.add_edge('Main.cpp', 'Main.obj', compile)
		g.add_edge('SomeClass.cpp', 'main.obj', compile)
		g.add_edge('Main.obj', 'SomeClass.obj', link)
