import unittest
import auto_graph

def list_to_nested_set(lst):
    if not lst:
        return set()  # Return an empty set if the input list is empty

    nested_set = set()
    for item in lst:
        if isinstance(item, list):
            nested_set.add(list_to_nested_set(item))
        else:
            nested_set.add(item)

    return nested_set

class GraphTest(unittest.TestCase):
    def test_0_simple_graph(self):
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

        set_gens = []
        for gen in gens:
            set_gens.append(list_to_nested_set(gen))
        
        truth_sets = [
            {'a'},
            {'b', 'c'},
            {'d', 'e'}
            ]

        for i in range(len(set_gens)):
            self.assertSetEqual(truth_sets[i], set_gens[i])