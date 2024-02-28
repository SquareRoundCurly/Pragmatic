import auto_graph

dir = 'tests/3_graph/multi_file'

def delete_file(file_path):
    import os
    if os.path.exists(file_path):
        os.remove(file_path)

def compile(g, from_node, to_node):
    import subprocess

    auto_graph.print(f'{from_node.name()} -> {to_node.name()}')
    subprocess.run(f'clang -c {dir}/{from_node.name()} -o {dir}/{to_node.name()}')

def link(g, from_node, to_node):
    import subprocess

    auto_graph.print(f'{from_node.name()} -> {to_node.name()}')
    subprocess.run(f'clang -fuse-ld=lld -o {dir}/{to_node.name()} {" ".join(f"{dir}/{node.name()}" for node in g.get_parents("objects"))}')

delete_file(f'{dir}/Main.o')
delete_file(f'{dir}/SomeClass.o')
delete_file(f'{dir}/SomeProgram.exe')

g = auto_graph.Graph()

g.add_node('Main.cpp')
g.add_node('SomeClass.cpp')

g.add_node('Main.o')
g.add_node('SomeClass.o')

g.add_edge('Main.cpp', 'Main.o', auto_graph.Task(compile, g))
g.add_edge('SomeClass.cpp', 'SomeClass.o', auto_graph.Task(compile, g))

g.add_node('objects')
g.add_edge('Main.o', 'objects')
g.add_edge('SomeClass.o', 'objects')

g.add_node('SomeProgram.exe')
g.add_edge('objects', 'SomeProgram.exe', auto_graph.Task(link, g))

g.run_tasks()

node = auto_graph.Node("new node")

auto_graph.print(node.name())

auto_graph.print(g.get_node('Main.cpp').name())


auto_graph.print(f'Parents of objects: {" ".join(node.name() for node in g.get_parents("objects"))}')




print(f'Result: {auto_graph.hash.store_and_verify_hash("run.py")}')
print(f'Result: {auto_graph.hash.store_and_verify_hash("run.py")}')
print(f'Result: {auto_graph.hash.store_and_verify_hash("setup.py")}')
print(f'Result: {auto_graph.hash.store_and_verify_hash("setup.py")}')




pass