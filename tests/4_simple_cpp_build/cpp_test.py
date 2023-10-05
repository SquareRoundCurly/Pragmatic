import auto_graph
import unittest
import subprocess

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
	print(f'Compiling: {source.get_name()} -> {target.get_name()}')
	return True

def link(source: auto_graph.Node, target: auto_graph.Node):
	parents = source.get_parents()
	parent_names = []
	for parent in parents:
		parent_names.append(parent.get_name())

	print(f'Linking: {" ".join(parent_names)} -> {target.get_name()}')

	return True

class NodesTest(unittest.TestCase):
	def test_0_node_references(self):
		g = auto_graph.Graph()

		g.add_edge('Main.cpp', 'Main.obj', compile)
		g.add_edge('SomeClass.cpp', 'SomeClass.obj', compile)
		
		g.add_edge('Main.obj', 'OBJs',)
		g.add_edge('SomeClass.obj', 'OBJs')

		g.add_edge('OBJs', 'Program.exe', link)

		success = g.exec()

		auto_graph.print(f'graph build {"succeeded" if success else "failed"}')