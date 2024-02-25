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
        dir = 'tests/3_graph/simple_build'

        def compile_simple_program(from_node, to_node):
            import subprocess
            
            auto_graph.print(f'{from_node.name()} -> {to_node.name()}')
            subprocess.run(f'clang {dir}/{from_node.name()} -o {dir}/{to_node.name()}')

        delete_file(f'{dir}/HelloWorld.exe')

        g = auto_graph.Graph()

        g.add_node('HelloWorld.cpp')
        g.add_node('HelloWorld.exe')

        g.add_edge('HelloWorld.cpp', 'HelloWorld.exe', auto_graph.Task(compile_simple_program))

        g.run_tasks()

        result = execute_command(f'{dir}/HelloWorld.exe')
        self.assertEqual('Hello World !\n', result.stdout)

    def test_2_multi_file_build(self):
        dir = 'tests/3_graph/multi_file'

        def delete_file(file_path):
            import os
            if os.path.exists(file_path):
                os.remove(file_path)

        def compile(from_node, to_node):
            import subprocess

            auto_graph.print(f'{from_node.name()} -> {to_node.name()}')
            subprocess.run(f'clang -c {dir}/{from_node.name()} -o {dir}/{to_node.name()}')

        def link(g: auto_graph.Graph, from_node, to_node):
            import subprocess

            auto_graph.print(f'{from_node.name()} -> {to_node.name()}')
            objs = f' '.join(f'{dir}/{node.name()}' for node in g.get_parents(from_node.name()))
            subprocess.run(f'clang -fuse-ld=lld -o {dir}/{to_node.name()} {objs}')

        delete_file(f'{dir}/Main.o')
        delete_file(f'{dir}/SomeClass.o')
        delete_file(f'{dir}/SomeProgram.exe')

        g = auto_graph.Graph()

        g.add_node('Main.cpp')
        g.add_node('SomeClass.cpp')

        g.add_node('Main.o')
        g.add_node('SomeClass.o')

        g.add_edge('Main.cpp', 'Main.o', auto_graph.Task(compile))
        g.add_edge('SomeClass.cpp', 'SomeClass.o', auto_graph.Task(compile))

        g.add_node('objects')
        g.add_edge('Main.o', 'objects')
        g.add_edge('SomeClass.o', 'objects')

        g.add_node('SomeProgram.exe')
        g.add_edge('objects', 'SomeProgram.exe', auto_graph.Task(link, g))

        g.run_tasks()

        result = execute_command(f'{dir}/SomeProgram.exe')
        self.assertEqual('42\n', result.stdout)