import auto_graph
import unittest
import subprocess
from pathlib import Path

src_dir = "tests\\4_simple_cpp_build\\src\\"

def run_shell_command(command: str):
	try:
		result = subprocess.run(command, shell=True, text=True, capture_output=True)
		
		if result.stdout:
			print("Output:")
			print(result.stdout)
		
		if result.stderr:
			print("Error:")
			print(result.stderr)
		
	except Exception as e:
		print(f"Failed to run command. Error: {e}")

run_shell_command('clang --version')

def compile(source: auto_graph.Node, target: auto_graph.Node):
	output = auto_graph.run_command(f'clang++ -c {src_dir}{source.get_name()} -o {src_dir}{target.get_name()}')
	print(f'Compiling: {source.get_name()} -> {target.get_name()}\n{output}')
	return True

def link(source: auto_graph.Node, target: auto_graph.Node):
	parents = source.get_parents()
	parent_names = []
	for parent in parents:
		parent_names.append(src_dir + parent.get_name())

	output = auto_graph.run_command(f'clang++ -fuse-ld=lld {" ".join(parent_names)} -o {src_dir}{target.get_name()}')
	print(f'Linking: {" ".join(parent_names)} -> {src_dir}{target.get_name()}\n{output}')

	return True

class NodesTest(unittest.TestCase):
	def test_0_node_references(self):
		g = auto_graph.Graph()

		g.add_edge('Main.cpp', 'Main.obj', compile)
		g.add_edge('SomeClass.cpp', 'SomeClass.obj', compile)
		
		g.add_edge('Main.obj', 'OBJs',)
		g.add_edge('SomeClass.obj', 'OBJs')

		g.add_edge('OBJs', 'Program.exe', link)

		g.print_topological_generations()

		result = g.exec()

		auto_graph.print(f'graph build {"succeeded" if result["success"] else "failed"}')
		auto_graph.print(f'graph checked {result["nodesVisited"]} nodes and ran {result["tasksRun"]} tasks')

		self.assertEqual(result["success"], True)

		output = auto_graph.run_command(f'{src_dir}Program.exe')
		self.assertEqual(output, 'Hello world !\n')