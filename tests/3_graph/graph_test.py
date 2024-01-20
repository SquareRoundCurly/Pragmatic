import unittest
import auto_graph

import os

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

def delete_file(file_path):
    if os.path.exists(file_path):
        os.remove(file_path)

def execute_command(command):
    import subprocess
    try:
        return subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, universal_newlines=True, check=True)
    
    except subprocess.CalledProcessError as e:
        return e

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

    def test_1_simple_build(self):
        dir = 'tests/3_graph'

        def compile_simple_program(from_node, to_node):
            import subprocess
            
            auto_graph.print(f'{from_node} -> {to_node}')
            subprocess.run(f'clang {dir}/{from_node} -o {dir}/{to_node}')

        delete_file(f'{dir}/HelloWorld.exe')

        g = auto_graph.Graph()

        g.add_node('HelloWorld.cpp')
        g.add_node('HelloWorld.exe')

        g.add_edge('HelloWorld.cpp', 'HelloWorld.exe', auto_graph.Task(compile_simple_program))

        g.run_tasks()

        result = execute_command(f'{dir}/HelloWorld.exe')
        self.assertEqual('Hello World !\n', result.stdout)