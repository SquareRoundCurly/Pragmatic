import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent.absolute()))

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

def compile():
	print('Compiling')
	return True

def link():
	print('Linking')
	return True

auto_graph.initialize()

g = auto_graph.Graph()

g.add_edge('Main.cpp', 'Main.obj', compile)
g.add_edge('SomeClass.cpp', 'main.obj', compile)
g.add_edge('Main.obj', 'SomeClass.obj', link)

g.exec()

auto_graph.cleanup()